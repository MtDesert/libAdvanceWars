#ifndef SPRITE_TERRAININFO_H
#define SPRITE_TERRAININFO_H

#include"GameString.h"
#include"Terrain.h"

class Sprite_TerrainInfo:public GameSprite{
public:
	Sprite_TerrainInfo();
	//数据相关
	void setTerrain(const TerrainCode &terrainCode,const Terrain &terrain);

	//控件相关
	ColorRGBA bgColor;//背景色
	GameSprite terrainIcon;//地形图标
	GameString terrainName;//地形名
	GameSprite starIcon;//星星图标
	GameString starNum;//星星数量
};
#endif//SPRITE_TERRAININFO_H