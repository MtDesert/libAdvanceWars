#include"Sprite_Unit.h"

Sprite_Unit::Sprite_Unit(){}

void Sprite_Unit::setUnit(const Unit &unit){
	auto texArr=texArray->data(unit.corpType);
	if(texArr){
		auto tex=texArr->data(unit.color);
		if(tex)texture=*tex;
	}
}