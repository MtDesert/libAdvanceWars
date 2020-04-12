#include"Sprite_UnitInfo.h"

#define ATTRIB(MACRO)\
MACRO(HP)\
MACRO(Gas)\
MACRO(Ammo)

Sprite_UnitInfo::Sprite_UnitInfo(){
	size.setXY(160,160);
	GameSprite::bgColor=&bgColor;
	bgColor=ColorRGBA(0,0,0,128);
	//调整大小
	unitName.setRawString("");
	unitIcon.size.y = unitName.size.y;
	//各个控件位置
	addSubObject(&unitName);
	addSubObject(&unitIcon);
#define ATTR(name) \
	addSubObject(&icon##name);\
	icon##name.addSubObject(&string##name);\
	icon##name.position.x=-96;
}

void Sprite_UnitInfo::setUnitData(const UnitData &unitData){}