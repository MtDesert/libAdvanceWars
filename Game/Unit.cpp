#include "Unit.h"

Unit::Unit(uint8 corpType,
		   uint8 troopType,
		   const decltype(coordinate) &coordinate,
		   uint8 healthPower,
		   uint8 fuel,
		   uint8 ammunition):
	ChessPiece(corpType,troopType,coordinate),
	healthPower(healthPower),
	fuel(fuel),
	ammunition(ammunition),
	progressValue(0),
	isVisible(true),isWait(false),isHide(false),level(0){}
bool Unit::operator==(const Unit &unit)const{
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
uint8 Unit::presentHP(uint8 hp){
	uint8 ret=hp/10;
	return hp%10>0?ret+1:ret;
}
uint8 Unit::presentHP()const{return presentHP(healthPower);}