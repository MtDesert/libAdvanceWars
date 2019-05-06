#ifndef DAMAGECACULATOR_H
#define DAMAGECACULATOR_H

/*#include"Terrain.h"
#include"Unit.h"

class DamageCaculator:public Unit
{
public:
	DamageCaculator();

	static void caculate_conflict(DamageCaculator &attacker,DamageCaculator &defender,bool caculateLuckDamage=true);
	static bool isCounterAttackable(const DamageCaculator &attacker,const DamageCaculator &defender);

	//Base data
	_List_const_iterator<Commander> commander;
	int co;
	Commander::PowerStatus powerStatus;
	bool statusAppend()const;
	//Weather::WeatherType weatherType;
	int terrainType;
	int amount_InstructionTower;

	//Derived data
	int corpDamage;
	int coAttackPercent;
	int coDefendPercent;
	int coLuckyDamage_Min;
	int coLuckyDamage;
	int coLuckyDamage_Max;
	int terrainDefendLevel;

	void caculate_derivedData();
	void caculate_derivedData(DamageCaculator &def,bool counterAttack);
	void caculate_derivedData_coLuckDamage_random();
	void caculate_derivedData_coLuckDamage_limit();

	//Caculate result
	int towerAttack;
	int terrainDefend;
	int attackPower;
	int defendPower;
	int finalDamage;
	int finalHP;
	int valueLost;

	bool caculate_Attack(DamageCaculator &defender);
};*/

#endif // DAMAGECACULATOR_H