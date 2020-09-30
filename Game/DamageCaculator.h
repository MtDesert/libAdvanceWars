#ifndef DAMAGECACULATOR_H
#define DAMAGECACULATOR_H

#include"Campaign.h"
#include"Commander.h"
#include"LuaState.h"

//前向声明
class Campaign;
class UnitData;
class DamageCaculator;
//损伤数据,主要用于各种计算
struct DamageData{
	DamageData();
	void clear();
	void setAttackerDefender(const DamageCaculator &caculator,const UnitData &attacker,const UnitData &defender);//根据攻击防御方来设置自身数据
	static int finalDamage(int baseDmg,int dmgFix,int presentHP,int attack,int defend);
	//基础数据
	const UnitData *unitData;//基准的单位数据
	//攻击
	int coAttack;//指挥官修正
	int baseAttack;//据点修正
	int customAttack;//自定义修正
	int powerAttack;//发动能力修正
	int attackPercent()const;//攻击百分比
	//防御
	int coDefence;//指挥官修正
	int terrainDefence;//地形防御修正
	int customDefence;//自定义修正
	int powerDefence;//发动能力修正
	int defendPercent()const;//防御百分比
	//损伤
	int baseDamage;//基本损伤
	IntRange damageRange;//损伤范围
	IntRange damageFix;//损伤修正
	int realDamageFix;//实际附加的损伤修正
	//得失
	int hpLost;//损伤的HP
	int priceLost;//损失的造价
};
//损伤计算器,本类主要负责计算,基本不负责存储
class DamageCaculator{
	int luaFunc_corpDamage(const string &attacker,const string &defender,int weaponIndex)const;//查询兵种间的基础损伤(攻击兵种名,防御兵种名,攻击武器索引)
public:
	DamageCaculator();

	LuaState *luaState;//执行代码规则文件
	Campaign *campaign;//一场战役,计算损伤时候就靠它提供的各种数据
	DamageData attackerDamageData,defenderDamageData;//交手双方的损伤数据

	//可攻击性
	bool canAttack(const UnitData &attacker,const UnitData &defender,const decltype(Unit::coordinate) &attackPos);//判断attacker能否攻击defender
	bool canCounterAttack(const UnitData &attacker,const UnitData &defender);//判断attacker能否反击defender
	//损伤
	int corpDamage(const Corp &atkCorp,const Corp &defCorp,int weaponIndex)const;//查询兵种损伤
	int unitDamage(const UnitData &attacker,const UnitData &defender)const;//查询单位损伤

	void attackPredict();//攻击预测,预测攻击过程中的各种数据
	void executeAttack();//开始执行攻击动作
	void attack(DamageData &atkDmg,DamageData &defDmg);//atkUnit对defUnit发动攻击
};

#endif//DAMAGECACULATOR_H