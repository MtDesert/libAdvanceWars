#ifndef SPRITE_UNIT_H
#define SPRITE_UNIT_H

#include"GameSprite.h"
#include"Unit.h"

//高级战争的单位精灵,用于渲染
class Sprite_Unit:public GameSprite{
public:
	Sprite_Unit();

	//单位纹理,用于渲染时候选择单位信息
	TextureCacheArray *texArray;
	//设置unit,并更新unit信息
	void setUnit(const Unit &unit);
};
#endif