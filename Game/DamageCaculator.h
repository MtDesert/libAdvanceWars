#ifndef DAMAGECACULATOR_H
#define DAMAGECACULATOR_H

#include"Campaign.h"
#include"Commander.h"
#include"LuaState.h"

//前向声明
class Campaign;
class UnitData;
//损伤计算器,本类主要负责计算,基本不负责存储
class DamageCaculator{
	int luaFunc_corpDamage(const string &attacker,const string &defender,int weaponIndex);//查询兵种间的基础损伤(攻击兵种名,防御兵种名,攻击武器索引)
public:
	DamageCaculator();

	LuaState luaState;//执行代码规则文件
	Campaign *campaign;//一场战役,计算损伤时候就靠它提供的各种数据

	//指挥官影响相关

	//可攻击性
	bool canAttack(UnitData &attacker,UnitData &defender,const decltype(Unit::coordinate) &attackPos);//判断attacker能否攻击defender
	bool canCounterAttack(UnitData &attacker,UnitData &defender);//判断attacker能否反击defender
	//损伤
	int corpDamage(const Corp &atkCorp,const Corp &defCorp,int weaponIndex);//查询兵种损伤
	int commanderDamage(const Corp &corp,const CommanderPower &power);//查询指挥官的兵种损伤
	int unitDamage(const Unit &atkUnit,const Unit &defUnit);//查询单位损伤
	int predictDamage(UnitData &attacker, UnitData &defender, int damageFix, bool isCounterAttak);//查询attacker对defender的预计损伤,damageFix进行修正

	void executeAttack();//开始执行攻击动作
	void attack(UnitData &attacker,UnitData &defender);//atkUnit对defUnit发动攻击
};

#endif//DAMAGECACULATOR_H