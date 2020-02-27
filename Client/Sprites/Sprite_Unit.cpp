#include"Sprite_Unit.h"

Sprite_Unit::Sprite_Unit(){}

void Sprite_Unit::setUnit(const Unit &unit){
	setTexture(texArray->getTexture(unit.corpType,unit.color));
}