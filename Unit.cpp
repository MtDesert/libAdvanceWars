#include "Unit.h"
#include<String.h>
#include<fstream>

Unit::Unit(CorpType corpType,
		   ColorType troopType,
		   const Point &coordinate,
		   uint8 healthPower,
		   uint8 fuel,
		   uint8 ammunition):
	ChessPiece(corpType,troopType,coordinate),
	healthPower(healthPower),
	fuel(fuel),
	ammunition(ammunition),
	progressValue(0),
	isVisible(true),isWait(false),isHide(false),level(0){}
bool Unit::operator==(const Unit &unit)const
{
	return ChessPiece::operator==(unit)
			&&healthPower==unit.healthPower
			&&fuel==unit.fuel
			&&ammunition==unit.ammunition
			//or material
			&&progressValue==unit.progressValue//capture or build
			&&isVisible==unit.isVisible
			&&isWait==unit.isWait
			&&isHide==unit.isHide
			&&level==unit.level;
}

uint8 Unit::presentHP(uint8 hp)
{
	uint8 ret=hp/10;
	return hp%10>0?ret+1:ret;
}
uint8 Unit::presentHP()const{return presentHP(healthPower);}

Unit* UnitList::findUnit_byCoordinate(short x,short y)
{
	Point p(x,y);
	for(_List_iterator<Unit> itr=this->begin();itr!=this->end();++itr)
	{
		if(itr->coordinate==p)return &(*itr);
	}
	return NULL;
}