#include"Sprite_TerrainInfo.h"
#include"Game_AdvanceWars.h"
#include"Number.h"

Sprite_TerrainInfo::Sprite_TerrainInfo(){
	size.setXY(160,128);
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
}

void Sprite_TerrainInfo::setUnitData(const UnitData &unitData){
	auto code=unitData.terrainCode;
	auto trn=unitData.terrain;
	if(code && trn){
		GAME_AW
		terrainIcon.setTexture(game->terrainsTexturesArray.getTexture(trn->terrainType,trn->status));
		terrainName.setString(code->translate);
		starNum.setString(Number::toString(code->defendLV));
	}
}