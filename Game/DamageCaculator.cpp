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

int DamageCaculator::corpDamage(const Corp &atkCorp,const Corp &defCorp,int weaponIndex){
	return luaFunc_corpDamage(atkCorp.name,defCorp.name,weaponIndex);
}
void DamageCaculator::executeAttack(){
	auto &attacker=campaign->selectedUnitData;
	auto &defender=campaign->cursorUnitData;
	//开始交手
	attack(attacker,defender);//先手方攻击
	attack(defender,attacker);//后手方反击
}
void DamageCaculator::attack(UnitData &attacker, UnitData &defender){
	//攻方
	int weaponIndex = attacker.unit->ammunition>0 ? 0 : 1;//确定要选用的主副武器
	auto baseDamage = luaFunc_corpDamage(attacker.corp->name,defender.corp->name,weaponIndex);//获取基础伤害值
	if(baseDamage<0)return;
	auto dmgFix = Number::randomInt(0,10);//损伤修正
	//被攻方
	auto defendPower = defender.terrainCode->defendLV * defender.unit->presentHP();//获取敌方防御
	auto effectiveDamagePercent = 100-defendPower;//计算有效损伤系数
	if(effectiveDamagePercent<0)effectiveDamagePercent=0;//有效损伤系数>=0
	//开始计算损伤(注:代码中统一把所有被除数相乘,所有除数相乘,最后再做除法,以减小误差)
	//公式:损伤=(基础损伤+损伤修正)*(攻击方表现HP/10)*(有效损伤系数/100)
	auto dmg = Number::divideRound((baseDamage + dmgFix)*attacker.unit->presentHP()*effectiveDamagePercent,1000);
	auto hp=defender.unit->healthPower;
	defender.unit->healthPower = dmg>hp ? 0 : hp-dmg;
	printf("HP=%u 基础损伤%d,修正%d,对方防御%d,实际损伤%d\n",attacker.unit->healthPower,baseDamage,dmgFix,defendPower,dmg);
}