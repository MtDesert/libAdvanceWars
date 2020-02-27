#include"Menu_BattleField.h"
#include"Game_AdvanceWars.h"
#include"Number.h"

#define ICON_SIZE 32
#define MENU_INIT \
if(!game)game=Game_AdvanceWars::currentGame();\
setRenderItemAmount(8);\
pSpriteSelector = new GameSprite();\
pSpriteSelector->position.x = -itemArray[0].size.x/2 + ICON_SIZE/2;

#define ITEM_ICON_NAME_NULL \
spriteIcon.setTexture(Texture());\
stringName.setString("");

static Game_AdvanceWars *game=nullptr;

MenuItem_CorpSelect::MenuItem_CorpSelect(){
	//尺寸
	size.setXY(ICON_SIZE*10,ICON_SIZE);
	setCursorWidth(ICON_SIZE);
	//位置
	auto rect=rectF();
	stringPrice.position.x=rect.p1.x;
	stringPrice.anchorPoint.x=1;
	//添加
	addSubObject(&stringPrice);
}
MenuItem_TerrainSelect::MenuItem_TerrainSelect(){
	size.setXY(ICON_SIZE*9,ICON_SIZE);
	setCursorWidth(ICON_SIZE);
}
MenuItem_TroopSelect::MenuItem_TroopSelect(){
	size.setXY(ICON_SIZE*8,ICON_SIZE);
	setCursorWidth(ICON_SIZE);
}
MenuItem_CorpCommand::MenuItem_CorpCommand(){
	size.setXY(ICON_SIZE*4,ICON_SIZE);
	setCursorWidth(ICON_SIZE);
}

Menu_CorpSelect::Menu_CorpSelect():troopID(2){MENU_INIT}
Menu_TerrainSelect::Menu_TerrainSelect():troopID(0){MENU_INIT}
Menu_TroopSelect::Menu_TroopSelect(){MENU_INIT}
Menu_CorpCommand::Menu_CorpCommand(){MENU_INIT}

void MenuItem_CorpSelect::updateData(SizeType pos){
	auto corp=game->mCorpsList.data(pos);//取数据
	if(corp){
		stringName.setString(corp->translate);
		stringPrice.setString(Number::toString(corp->price));
		auto menu=dynamic_cast<Menu_CorpSelect*>(parentObject);
		if(menu){
			spriteIcon.setTexture(game->corpsTexturesArray.getTexture(pos,menu->troopID));
		}
	}else{
		ITEM_ICON_NAME_NULL
	}
}
void MenuItem_TerrainSelect::updateData(SizeType pos){
	auto terrainCode=game->mTerrainCodesList.data(pos);
	if(terrainCode){
		stringName.setString(terrainCode->translate);
		auto menu=dynamic_cast<Menu_TerrainSelect*>(parentObject);
		if(menu){
			auto status = menu->troopID;
			if(!terrainCode->capturable)status=0;//非据点,就用地形的默认图块
			spriteIcon.setTexture(game->terrainsTexturesArray.getTexture(pos,status));
		}
	}else{
		ITEM_ICON_NAME_NULL
	}
}
void MenuItem_TroopSelect::updateData(SizeType pos){
	auto troop=game->mTroopsList.data(pos);//取数据
	if(troop){
		stringName.setString(troop->translate);
		auto tex=game->troopsTextures.data(pos);
		spriteIcon.setTexture(tex ? *tex : Texture());
	}else{
		ITEM_ICON_NAME_NULL
	}
}

void MenuItem_CorpCommand::updateData(SizeType pos){
	if(pos<Campaign::AmountOfCorpEnumMenu){
		auto tex=game->corpMenuTextures.data(pos);
		spriteIcon.setTexture(tex ? *tex : Texture());
	}else{
		ITEM_ICON_NAME_NULL
	}
}

SizeType Menu_CorpSelect::rowAmount()const{return game->mCorpsList.size();}
SizeType Menu_TerrainSelect::rowAmount()const{return game->mTerrainCodesList.size();}
SizeType Menu_TroopSelect::rowAmount()const{return game->mTroopsList.size();}
SizeType Menu_CorpCommand::rowAmount()const{return Campaign::AmountOfCorpEnumMenu;}