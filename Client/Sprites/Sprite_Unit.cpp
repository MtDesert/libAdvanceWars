#include"Sprite_Unit.h"

Sprite_Unit::Sprite_Unit():unitTexArray(nullptr),numTexArray(nullptr){
	//添加hp标记
	spriteHP.position.setXY(28,4);
	addSubObject(&spriteHP);
}

void Sprite_Unit::setUnit(const Unit &unit){
	setTexture(unitTexArray->getTexture(unit.corpType,unit.color));
	setColor(unit.isWait ? ColorRGBA::Gray : ColorRGBA::White);
	//决定是否显示hp
	spriteHP.setTexture(numTexArray->getTexture(unit.presentHP()));
}