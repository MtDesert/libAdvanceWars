#ifndef SPRITE_TERRAININFO_H
#define SPRITE_TERRAININFO_H

#include"GameString.h"
#include"Campaign.h"

class Sprite_TerrainInfo:public GameSprite{
public:
	Sprite_TerrainInfo();

	void setUnitData(const UnitData &unitData);//设置显示数据

	//控件相关
	ColorRGBA bgColor;//背景色
	GameSprite terrainIcon;//地形图标
	GameString terrainName;//地形名
	GameSprite starIcon;//星星图标
	GameString starNum;//星星数量
};
#endif//SPRITE_TERRAININFO_H