#include"DamageCaculator.h"
#include"Number.h"

DamageCaculator::DamageCaculator():luaState(nullptr),campaign(nullptr){}
DamageData::DamageData(){clear();}

void DamageData::clear(){
	unitData=nullptr;
	coAttack=baseAttack=powerAttack=customAttack=0;
	coDefence=terrainDefence=powerDefence=customDefence=0;
	baseDamage=damageFix.minimun=damageFix.maximun=realDamageFix=0;
	hpLost=priceLost=0;
}
void DamageData::setAttackerDefender(const DamageCaculator &caculator,const UnitData &attacker,const UnitData &defender){
	clear();
	unitData=&attacker;
	//攻击方
	auto damage=caculator.unitDamage(attacker,defender);
	if(damage<0)return;//不可攻击
	//攻防要算上指挥官的影响
	auto feature=attacker.campaign->getCommanderPowerFeature(attacker);
	//攻击力加成
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
	//计算损伤
	damage=Number::divideRound(damage*(100 + this->coAttack + this->baseAttack + this->powerAttack + this->customAttack),100);
	//防御方
	this->coDefence = feature.defence;//CO防御
	if(defender.corp->isDirectAttack())this->coDefence += feature.directDefence;//对方为直接攻击,则使用直接防御
	if(defender.corp->isIndirectAttack())this->coDefence += feature.indirectDefence;//对方为间接攻击,则使用间接防御
	this->terrainDefence = attacker.terrainCode->defendLV * attacker.unit->presentHP();//地形防御力
	//其它加成
	auto defence = this->coDefence + this->terrainDefence + this->powerDefence + this->customDefence;
	//有效损伤系数
	auto effectiveDamagePercent = 100-defence;//计算有效损伤系数
	if(effectiveDamagePercent<0)effectiveDamagePercent=0;//有效损伤系数>=0
	//开始计算损伤(注:代码中统一把所有被除数相乘,所有除数相乘,最后再做除法,以减小误差)
	//公式:损伤=(基础损伤+损伤修正)*(攻击方表现HP/10)*(有效损伤系数/100)
	baseDamage = Number::divideRound(damage*attacker.unit->presentHP()*effectiveDamagePercent,1000);
	damageFix = feature.damageFix;
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
bool DamageCaculator::canCounterAttack(UnitData &attacker,UnitData &defender){
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
	auto &attacker=campaign->selectedUnitData;
	auto &defender=campaign->cursorUnitData;
	//计算交手数据
	attackerDamageData.setAttackerDefender(*this,attacker,defender);//计算攻击者数据
	defenderDamageData.setAttackerDefender(*this,defender,attacker);//计算防御者数据
	//开始交手
	attack(attacker,defender);//先手方攻击
	if(defender.unit->healthPower>0 && canCounterAttack(defender,attacker)){//存活且可反击
		attack(defender,attacker);//后手方反击
	}
}
void DamageCaculator::attack(UnitData &attacker,UnitData &defender){
	auto atkFeature=campaign->getCommanderPowerFeature(attacker);
	printf("损伤范围%d~%d\n",atkFeature.damageFix.minimun,atkFeature.damageFix.maximun);
	auto dmgFix = Number::randomInt(atkFeature.damageFix.minimun,atkFeature.damageFix.maximun);//损伤修正
	auto dmg = 0;//predictDamage(attacker,defender,dmgFix,false);
	auto hp = defender.unit->healthPower;
	defender.unit->healthPower = dmg>hp ? 0 : hp-dmg;
	//输出调试信息
	printf("%s部队-%s-攻击-%s部队-%s,HP=%u 实际损伤%d(修正%d),对方剩余HP=%u\n",
		attacker.troop->name.data(),
		attacker.corp->name.data(),
		defender.troop->name.data(),
		defender.corp->name.data(),
		attacker.unit->healthPower,dmg,dmgFix,defender.unit->healthPower);
}