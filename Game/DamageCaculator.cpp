#include"DamageCaculator.h"
#include"Number.h"

DamageCaculator::DamageCaculator():luaState(nullptr),campaign(nullptr){}
DamageData::DamageData(){clear();}

void DamageData::clear(){
	unitData=nullptr;
	coAttack=baseAttack=powerAttack=customAttack=0;
	coDefence=terrainDefence=powerDefence=customDefence=0;
	baseDamage=damageRange.minimun=damageRange.maximun=0;
	damageFix.minimun=damageFix.maximun=realDamageFix=0;
	hpLost=priceLost=0;
}
void DamageData::setAttackerDefender(const DamageCaculator &caculator,const UnitData &attacker,const UnitData &defender){
	clear();
	unitData=&attacker;
	//攻击方
	auto damage=caculator.unitDamage(attacker,defender);
	if(damage<0)return;//不可攻击
	//攻击力加成
	auto feature=attacker.campaign->getCommanderPowerFeature(attacker);
	this->coAttack = feature.attack;//指挥官的条件加成
	attacker.campaignTroop->allCapturableTerrains.forEach([&](const CampaignTroop::TerrainAmount &ta){//据点攻击加成
		auto code=attacker.campaign->battleField->terrainsList->data(ta.terrainID);
		if(!code)return;
		if(code->attackPercent)this->baseAttack += code->attackPercent * ta.amount;
	});
	if(attacker.campaignCO && attacker.campaignCO->powerLevel>0){//发动能力加成
		auto &rule=caculator.campaign->campaignRule;
		this->powerAttack = rule.attackFixWhenCOpower;
		this->powerDefence = rule.defenceFixWhenCOpower;
	}
	auto attack = attackPercent();
	//防御方防御加成
	this->coDefence = feature.defence;//CO防御
	if(defender.corp->isDirectAttack())this->coDefence += feature.directDefence;//对方为直接攻击,则使用直接防御
	if(defender.corp->isIndirectAttack())this->coDefence += feature.indirectDefence;//对方为间接攻击,则使用间接防御
	this->terrainDefence = attacker.terrainCode->defendLV * attacker.unit->presentHP();//地形防御力
	auto defence = defendPercent();
	//计算损伤
	damageFix=feature.damageFix;
	baseDamage=damage;
	damageRange.minimun=finalDamage(damage,damageFix.minimun,attacker.unit->presentHP(),attack,defence);
	damageRange.maximun=finalDamage(damage,damageFix.maximun,attacker.unit->presentHP(),attack,defence);
}
int DamageData::attackPercent()const{return coAttack + baseAttack + powerAttack + customAttack;}
int DamageData::defendPercent()const{return coDefence + terrainDefence + powerDefence + customDefence;}
int DamageData::finalDamage(int baseDmg,int dmgFix,int presentHP,int attack,int defend){
	//(注:代码中统一把所有被除数相乘,所有除数相乘,最后再做除法,以减小误差)
	//威力 = 兵种基础损伤*(100%+攻击修正百分比)+损伤修正
	auto dmg=Number::divideRound(baseDmg*(100+attack),100)+dmgFix;
	//有效损伤系数 = 100%-防御修正百分比
	auto effectiveDamagePercent = max(0,100-defend);
	//最终损伤=威力*(攻击方表现HP/10)*(有效损伤系数/100)
	return Number::divideRound(dmg * presentHP * effectiveDamagePercent,1000);
}

int DamageCaculator::luaFunc_corpDamage(const string &attacker,const string &defender,int weaponIndex)const{
	int ret=-1;
	if(luaState->getGlobalFunction("corpDamage")){
		luaState->push(attacker).push(defender).push(weaponIndex);
		if(luaState->protectCall()){
			luaState->getTopInteger(ret);
		}
	}
	return ret;
}

bool DamageCaculator::canAttack(const UnitData &attacker,const UnitData &defender,const decltype(Unit::coordinate) &attackPos){
	//先检查射程
	bool ret=false;
	auto dist=(attackPos-defender.unit->coordinate).manhattanLength();//得到距离
	auto &atkCorp=attacker.corp;
	if(atkCorp->isDirectAttack())ret=(dist<=1);//直接攻击部队只能攻击相邻的
	if(atkCorp->isIndirectAttack() && attacker.unit->coordinate==attackPos){//间接攻击部队不能移动
		auto wpn=atkCorp->firstAttackableWeapon();
		ret=wpn && (wpn->minRange<=dist && dist<=wpn->maxRange);
	}
	if(!ret)return ret;
	//剩下的就看武器能否攻击对方了
	return unitDamage(attacker,defender);
}
bool DamageCaculator::canCounterAttack(const UnitData &attacker,const UnitData &defender){
	if(canAttack(attacker,defender,attacker.unit->coordinate)){//要能反击,首先要能攻击
		return (attacker.unit->coordinate-defender.unit->coordinate).manhattanLength()<=1;//必须是近身才能反击
	}
	return false;
}
int DamageCaculator::corpDamage(const Corp &atkCorp,const Corp &defCorp,int weaponIndex)const{
	return luaFunc_corpDamage(atkCorp.name,defCorp.name,weaponIndex);
}
int DamageCaculator::unitDamage(const UnitData &attacker,const UnitData &defender)const{
	auto damage=-1;
	int sz=attacker.corp->weapons.size();
	for(auto idx=0;idx<sz;++idx){
		damage = corpDamage(*attacker.corp,*defender.corp,idx);//获取基础伤害值
		if(damage>=0){//找到了,找不到就换武器继续试试看
			if(idx==0 && attacker.unit->ammunition<=0)continue;//主武器用完了,还是得换武器
			break;
		}
	}
	return damage;
}

void DamageCaculator::attackPredict(){
	auto &attacker=campaign->selectedUnitData;
	auto &defender=campaign->cursorUnitData;
	//计算交手数据
	attackerDamageData.setAttackerDefender(*this,attacker,defender);//计算攻击者数据
	defenderDamageData.setAttackerDefender(*this,defender,attacker);//计算防御者数据
}
void DamageCaculator::executeAttack(){
	attackPredict();//攻击前要进行预计
	attack(attackerDamageData,defenderDamageData);//先手方攻击
	if(defenderDamageData.unitData->unit->healthPower>0 && canCounterAttack(*defenderDamageData.unitData,*attackerDamageData.unitData)){//存活且可反击
		attack(defenderDamageData,attackerDamageData);//后手方反击
	}
}
void DamageCaculator::attack(DamageData &atkDmg,DamageData &defDmg){
	printf("损伤范围%d~%d\n",atkDmg.damageRange.minimun,atkDmg.damageRange.maximun);
	auto fix=atkDmg.damageFix;
	auto dmgFix = Number::randomInt(fix.minimun,fix.maximun);//得到随机修正结果
	auto finalDmg = max(0,DamageData::finalDamage(atkDmg.baseDamage,dmgFix,atkDmg.unitData->unit->presentHP(),atkDmg.attackPercent(),defDmg.defendPercent()));//得到最终损伤
	//开始减少hp
	auto defUnit=defDmg.unitData->unit;
	auto hp=defUnit->healthPower;
	defUnit->healthPower=max(0,defUnit->healthPower-finalDmg);//保证HP为非负数
	//输出调试信息
	printf("%s部队%s-攻击-%s部队%s 威力%d 实际损伤%d,HP变化=%u->%d\n",
		atkDmg.unitData->troop->name.data(),
		atkDmg.unitData->corp->name.data(),
		defDmg.unitData->troop->name.data(),
		defDmg.unitData->corp->name.data(),
		finalDmg,hp-defUnit->healthPower,hp,defUnit->healthPower);
}