#include"DamageCaculator.h"
#include"Number.h"

DamageCaculator::DamageCaculator(){}

int DamageCaculator::luaFunc_corpDamage(const string &attacker,const string &defender,int weaponIndex){
	int ret=-1;
	if(luaState.getGlobalFunction("corpDamage")){
		luaState.push(attacker).push(defender).push(weaponIndex);
		if(luaState.protectCall()){
			luaState.getTopInteger(ret);
		}
	}
	return ret;
}

bool DamageCaculator::canAttack(UnitData &attacker,UnitData &defender,const decltype(Unit::coordinate) &attackPos){
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
	return predictDamage(attacker,defender,0)>=0;
}
bool DamageCaculator::canCounterAttack(UnitData &attacker,UnitData &defender){
	if(canAttack(attacker,defender,attacker.unit->coordinate)){//要能反击,首先要能攻击
		return (attacker.unit->coordinate-defender.unit->coordinate).manhattanLength()<=1;//必须是近身才能反击
	}
	return false;
}
int DamageCaculator::corpDamage(const Corp &atkCorp,const Corp &defCorp,int weaponIndex){
	return luaFunc_corpDamage(atkCorp.name,defCorp.name,weaponIndex);
}
int DamageCaculator::unitDamage(const Unit &atkUnit,const Unit &defUnit){return 0;}
int DamageCaculator::predictDamage(UnitData &attacker,UnitData &defender,int damageFix){
	//攻击方
	auto baseDamage=0;
	for(SizeType idx=0;idx<attacker.corp->weapons.size();++idx){
		baseDamage = luaFunc_corpDamage(attacker.corp->name,defender.corp->name,idx);//获取基础伤害值
		if(baseDamage>=0){//找到了,找不到就换武器继续试试看
			if(idx==0 && attacker.unit->ammunition<=0)continue;//主武器用完了,还是得换武器
			break;
		}
	}
	if(baseDamage<0)return baseDamage;//不可攻击
	//防御方
	auto defendPower = defender.terrainCode->defendLV * defender.unit->presentHP();//获取敌方防御
	auto effectiveDamagePercent = 100-defendPower;//计算有效损伤系数
	if(effectiveDamagePercent<0)effectiveDamagePercent=0;//有效损伤系数>=0
	//开始计算损伤(注:代码中统一把所有被除数相乘,所有除数相乘,最后再做除法,以减小误差)
	//公式:损伤=(基础损伤+损伤修正)*(攻击方表现HP/10)*(有效损伤系数/100)
	return Number::divideRound((baseDamage+damageFix)*attacker.unit->presentHP()*effectiveDamagePercent,1000);
}

void DamageCaculator::executeAttack(){
	auto &attacker=campaign->selectedUnitData;
	auto &defender=campaign->cursorUnitData;
	//开始交手
	attack(attacker,defender);//先手方攻击
	if(canCounterAttack(defender,attacker)){
		attack(defender,attacker);//后手方反击
	}
}
void DamageCaculator::attack(UnitData &attacker,UnitData &defender){
	auto dmgFix = Number::randomInt(0,10);//损伤修正
	auto dmg = predictDamage(attacker,defender,dmgFix);
	auto hp=defender.unit->healthPower;
	defender.unit->healthPower = dmg>hp ? 0 : hp-dmg;
	//输出调试信息
	printf("%s部队-%s-攻击-%s部队-%s,HP=%u 实际损伤%d(修正%d),对方剩余HP=%u\n",
		attacker.troop->name.data(),
		attacker.corp->name.data(),
		defender.troop->name.data(),
		defender.corp->name.data(),
		attacker.unit->healthPower,dmg,dmgFix,defender.unit->healthPower);
}