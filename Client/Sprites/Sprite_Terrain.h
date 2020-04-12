#ifndef SPRITE_TERRAIN_H
#define SPRITE_TERRAIN_H

#include"GameSprite.h"
#include"Terrain.h"

/**精灵-地形,用来显示地形*/
class Sprite_Terrain:public GameSprite{
public:
	Sprite_Terrain();

	TextureCacheArray *terrainsTexArray;//地形纹理数组
	void setTerrain(const Terrain &terrain);//设置地形状态
};
#endif