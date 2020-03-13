#include"Sprite_TerrainInfo.h"
#include"Game_AdvanceWars.h"
#include"Number.h"

Sprite_TerrainInfo::Sprite_TerrainInfo(){
	size.setXY(128,128);
	GameSprite::bgColor=&bgColor;
	bgColor=ColorRGBA(0,0,0,128);
	//各个控件位置
	terrainName.position.y=48;
	starIcon.position.setXY(-16,-48);
	starNum.position.x=32;

	addSubObject(&terrainIcon);
	addSubObject(&terrainName);
	addSubObject(&starIcon);
	starIcon.addSubObject(&starNum);
	//设置星星图标
	GAME_AW
	starIcon.setTexture(game->texStar);
}

void Sprite_TerrainInfo::setTerrain(const TerrainCode &terrainCode,const Terrain &terrain){
	GAME_AW
	//图标
	terrainIcon.setTexture(game->terrainsTexturesArray.getTexture(terrain.terrainType,terrain.status));
	terrainName.setString(terrainCode.translate);
	starNum.setString(Number::toString(terrainCode.defendLV));
}