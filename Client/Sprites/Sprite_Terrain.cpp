#include "Sprite_Terrain.h"

Sprite_Terrain::Sprite_Terrain():terrainsTexArray(nullptr){
	anchorPoint.y=0;
}

void Sprite_Terrain::setTerrain(const Terrain &terrain){
	setTexture(terrainsTexArray->getTexture(terrain.terrainType,terrain.status));//设置纹理
}