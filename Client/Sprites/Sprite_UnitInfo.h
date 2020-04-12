#ifndef SPRITE_UNITINFO_H
#define SPRITE_UNITINFO_H

#include"GameString.h"
#include"Campaign.h"
class Sprite_UnitInfo:public GameSprite{
public:
	Sprite_UnitInfo();

	ColorRGBA bgColor;//背景色
	//各个显示控件
	GameSprite unitIcon;//单位图标
	GameString unitName;//单位名
	//HP,燃料,弹药
	GameSprite iconHP,iconGas,iconAmmo;
	GameString stringHP,stringGas,stringAmmo;

	void setUnitData(const UnitData &unitData);//显示单位数据
};
#endif//SPRITE_UNITINFO_H