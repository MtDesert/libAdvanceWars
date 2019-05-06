/*#include "DamageCaculator.h"
#include"Campaign.h"
#include"stdlib.h"
#include<iostream>

DamageCaculator::DamageCaculator():
	Unit(0),
	co(0),
	powerStatus(Commander::NoPower),
	//weatherType(Weather::NormalWeather),
	terrainType(0),
	amount_InstructionTower(0)
{
	corpDamage=-1;
	coAttackPercent=0;
	coDefendPercent=0;
	coLuckyDamage_Min=0;
	coLuckyDamage=0;
	coLuckyDamage_Max=10;
	towerAttack=0;
	terrainDefendLevel=0;
	terrainDefend=0;
	attackPower=0;
	defendPower=0;
	finalDamage=0;
	finalHP=healthPower;
	valueLost=0;
}

void DamageCaculator::caculate_conflict(DamageCaculator &attacker,DamageCaculator &defender,bool caculateLuckDamage)
{
	//caculate
	attacker.caculate_derivedData();
	defender.caculate_derivedData();
	attacker.caculate_derivedData(defender,false);
	defender.caculate_derivedData(attacker,true);
	if(caculateLuckDamage)
	{
		srand(time(NULL));
		attacker.caculate_derivedData_coLuckDamage_random();
		defender.caculate_derivedData_coLuckDamage_random();
	}
	else
	{
		attacker.caculate_derivedData_coLuckDamage_limit();
		defender.caculate_derivedData_coLuckDamage_limit();
	}

	//check who attack first
	bool counterAttackFirst=false;
	if(defender.co==Commander::Sonja&&defender.powerStatus==Commander::SuperPower)
	{
		counterAttackFirst=true;
	}

	int atkHP=attacker.healthPower;
	int defHP=defender.healthPower;
	//Counter Attack
	if(counterAttackFirst)
	{
		if(isCounterAttackable(attacker,defender))
		{
			//cout<<"Counter attack first"<<endl;cout.flush();
			defender.caculate_Attack(attacker);
			if(attacker.finalHP==0)return;
		}
	}
	//attack
	//cout<<"Attack"<<endl;cout.flush();
	attacker.healthPower=attacker.finalHP;
	attacker.caculate_Attack(defender);
	attacker.healthPower=atkHP;//recover
	if(defender.finalHP==0)return;
	//Counter Attack
	if(!counterAttackFirst)
	{
		if(isCounterAttackable(attacker,defender))
		{
			//cout<<"Counter attack"<<endl;cout.flush();
			defender.healthPower=defender.finalHP;
			defender.caculate_Attack(attacker);
			defender.healthPower=defHP;//recover
			if(attacker.finalHP==0)return;
		}
	}
}

bool DamageCaculator::isCounterAttackable(const DamageCaculator &attacker, const DamageCaculator &defender)
{
	bool conditionDirectAttack=
			defender.corp->isDirectAttack()&&
			(attacker.coordinate-defender.coordinate).manhattanLength()==1;
	bool conditionAntiTank=
			(defender.corpType==Corp::AntiTank||defender.corpType==Corp::AntiAirArtillery)&&
			(attacker.coordinate-defender.coordinate).manhattanLength()<=
			CorpList::defaultCorpsList.iterate(defender.corpType)->mainWeapon.maxRange+
			Commander::attackRangeMax(defender.co,(Corp::CorpType)defender.corpType,defender.powerStatus);
	return conditionDirectAttack||conditionAntiTank;
	return true;
}

bool DamageCaculator::statusAppend()const{return powerStatus!=Commander::NoPower;}

void DamageCaculator::caculate_derivedData()
{
	corp=CorpList::defaultCorpsList.iterate(corpType);
	if(corp==CorpList::defaultCorpsList.end())return;
	commander=Commander::defaultCommanderList.iterate(co);
	if(commander==Commander::defaultCommanderList.end())return;
	terrainAttribute=TerrainAttributeList::defaultList.iterate(terrainType);
	if(terrainAttribute==TerrainAttributeList::defaultList.end())return;

	//coAttackPercent=Commander::attack(*commander,*corp,powerStatus,weatherType,*terrainAttribute);
	//coDefendPercent=Commander::defend(co,(Corp::CorpType)corpType,powerStatus,weatherType,terrainType);
	//coLuckyDamage_Min=Commander::luckyDamage_LowerLimit(co,(Corp::CorpType)corpType,powerStatus);
	//coLuckyDamage_Max=Commander::luckyDamage_UpperLimit(co,(Corp::CorpType)corpType,powerStatus);
	//terrainDefendLevel=Terrain::defendLevel(terrainType);
	//clean final result
	towerAttack=amount_InstructionTower*10;
	terrainDefend=0;
	attackPower=0;
	defendPower=0;
	finalDamage=0;
	finalHP=healthPower;
	valueLost=0;
}
void DamageCaculator::caculate_derivedData(DamageCaculator &def,bool counterAttack)
{
	//adjust by condition!!!
	if(co==Commander::Colin&&powerStatus==Commander::SuperPower)
	{
		if(troop!=NULL)coAttackPercent+=troop->funds/300;
	}
	if(co==Commander::Javier)
	{
		switch(powerStatus)
		{
			case Commander::Power:
				towerAttack*=2;
				break;
			case Commander::SuperPower:
				towerAttack*=3;
				break;
		}
	}
	if(co==Commander::Kanbei&&powerStatus==Commander::SuperPower&&counterAttack)
	{
		coAttackPercent*=2;
	}
	if(co==Commander::Sonja&&counterAttack)
	{
		switch(powerStatus)
		{
			case Commander::NoPower:
				coAttackPercent+=20;
				break;
			case Commander::Power:case Commander::SuperPower:
				coAttackPercent+=50;
				break;
		}
	}
	if(co==Commander::Kindle&&powerStatus==Commander::SuperPower)
	{
		if(troop!=NULL)
		{
			coAttackPercent+=troop->baseAmount*10;
		}
	}

	//adjust by condition
	if(def.co==Commander::Lash&&def.powerStatus==Commander::SuperPower)
	{
		def.terrainDefendLevel*=2;
	}
	if(co==Commander::Sonja)//Sonja reduce terrain effect
	{
		switch(powerStatus)
		{
			case Commander::NoPower:
				def.terrainDefendLevel-=1;
				break;
			case Commander::Power:
				def.terrainDefendLevel-=2;
				break;
			case Commander::SuperPower:
				def.terrainDefendLevel-=3;
				break;
		}
	}
	if(def.co==Commander::Javier)
	{
		if(Corp::isIndirectAttack(corpType))
		{
			switch(def.powerStatus)
			{
				case Commander::NoPower:
					def.coDefendPercent+=20;
					break;
				case Commander::Power:
					def.coDefendPercent+=50;
					break;
				case Commander::SuperPower:
					def.coDefendPercent+=90;
					break;
			}
		}
		int towerDef=def.amount_InstructionTower*10;
		switch(def.powerStatus)
		{
			case Commander::Power:
				towerDef*=2;
				break;
			case Commander::SuperPower:
				towerDef*=3;
				break;
		}
		def.coDefendPercent+=towerDef;
	}
}
void DamageCaculator::caculate_derivedData_coLuckDamage_random()
{
	if(coLuckyDamage_Min<coLuckyDamage_Max)
	{
		int value=rand()%(coLuckyDamage_Max-coLuckyDamage_Min);
		coLuckyDamage=coLuckyDamage_Min=value;
	}
	else if(coLuckyDamage_Min=coLuckyDamage_Max)
	{
		coLuckyDamage=coLuckyDamage_Min;
	}
}
void DamageCaculator::caculate_derivedData_coLuckDamage_limit()
{
	if(coLuckyDamage>=coLuckyDamage_Max)coLuckyDamage=coLuckyDamage_Max-1;
	if(coLuckyDamage<coLuckyDamage_Min)coLuckyDamage=coLuckyDamage_Min;
}

//attack!!!
bool DamageCaculator::caculate_Attack(DamageCaculator &defender)
{
	int weaponCount=CorpList::defaultCorpsList.iterate(corpType)->weapons.size();
	bool hasWeapon=false;
	for(int i=0;i<weaponCount;++i)
	{
		hasWeapon=CorpList::defaultCorpsList.damageValue(corpType,defender.corpType,i,corpDamage);
		if(hasWeapon)break;
	}
	if(!hasWeapon)
	{
		corpDamage=-1;
		defender.finalHP=defender.healthPower;
		defender.valueLost=0;
		return false;
	}
	//attack power
	attackPower=
			(corpDamage*(100+coAttackPercent+(statusAppend()?10:0)+
			towerAttack)+100*coLuckyDamage)*presentHP()/1000;
	attackPower=max(attackPower,0);//make sure attack power >= 0

	//defend power
	defender.terrainDefend=defender.terrainDefendLevel*defender.presentHP();
	defender.defendPower=
			defender.coDefendPercent+
			//(Corp::isAirForce(defender.corpType)?0:defender.terrainDefend)+
			(defender.statusAppend()?10:0);

	//attack!!!!
	finalDamage=attackPower*(100-defender.defendPower)/100;//AWDS version
	//finalDamage=attackPower/(100+def.defendPower);//AWDOR version
	finalDamage=max(finalDamage,0);

	defender.finalHP=defender.healthPower-finalDamage;
	defender.finalHP=max(defender.finalHP,0);

	//value lost
	_List_const_iterator<Corp> corp=CorpList::defaultCorpsList.iterate(defender.corpType);
	int buildPrice=Commander::builtPrice(defender.co,defender.corpType,defender.powerStatus);
	defender.valueLost=corp->price*(100+buildPrice)*(defender.healthPower-defender.finalHP)/10000;
	return true;
}
//do attack action
bool Campaign::execute_Fire()
{
	inputDamageCaculator(*selectedUnit,attacker);
	inputDamageCaculator(*selectedTarget,defender);
	//do!!!
	DamageCaculator::caculate_conflict(attacker,defender);
	//write data
	selectedUnit->healthPower=attacker.finalHP;
	selectedTarget->healthPower=defender.finalHP;
	if(selectedUnit->healthPower==0)battleField.deleteUnit(selectedUnit);
	if(selectedTarget->healthPower==0)battleField.deleteUnit(selectedTarget);
	return true;
}*/