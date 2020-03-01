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

//Item们的构造函数
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
MenuItem_MapEdit::MenuItem_MapEdit(){
	size.setXY(ICON_SIZE*6,ICON_SIZE);
	setCursorWidth(ICON_SIZE);
}
MenuItem_CorpCommand::MenuItem_CorpCommand(){
	size.setXY(ICON_SIZE*4,ICON_SIZE);
	setCursorWidth(ICON_SIZE);
}

//Menu们的构造函数
Menu_CorpSelect::Menu_CorpSelect():menuTroopSelect(nullptr){MENU_INIT}
Menu_TerrainSelect::Menu_TerrainSelect():menuTroopSelect(nullptr){MENU_INIT}
Menu_TroopSelect::Menu_TroopSelect(){MENU_INIT}
Menu_MapEdit::Menu_MapEdit():menuCorpSelect(nullptr),menuTerrainSelect(nullptr),menuTroopSelect(nullptr){MENU_INIT}
Menu_CorpCommand::Menu_CorpCommand(){MENU_INIT}

//Item们的updateData函数
void MenuItem_CorpSelect::updateData(SizeType pos){
	auto corp=game->mCorpsList.data(pos);//取数据
	if(corp){
		stringName.setString(corp->translate);
		stringPrice.setString(Number::toString(corp->price));
		auto menu=dynamic_cast<Menu_CorpSelect*>(parentObject);
		if(menu){
			spriteIcon.setTexture(game->corpsTexturesArray.getTexture(pos,menu->menuTroopSelect->selectingItemIndex));
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
			auto status = menu->menuTroopSelect->selectingItemIndex;
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
		spriteIcon.setTexture(game->troopsTextures.getTexture(pos));
	}else{
		ITEM_ICON_NAME_NULL
	}
}

void MenuItem_MapEdit::updateData(SizeType pos){
	auto menu=dynamic_cast<Menu_MapEdit*>(parentObject);
	if(menu && pos< Scene_BattleField::AmountOfEnumMapEditCommand){
		switch(pos){
			case Scene_BattleField::MapEdit_CorpSelect:
				spriteIcon.setTexture(game->corpsTexturesArray.getTexture(
					menu->menuCorpSelect->selectingItemIndex,
					menu->menuTroopSelect->selectingItemIndex));
			break;
			case Scene_BattleField::MapEdit_TerrainSelect:{
				auto terrainCode=game->mTerrainCodesList.data(menu->menuTerrainSelect->selectingItemIndex);
				if(terrainCode){
					auto status = menu->menuTroopSelect->selectingItemIndex;
					if(!terrainCode->capturable)status=0;//非据点,就用地形的默认图块
					spriteIcon.setTexture(game->terrainsTexturesArray.getTexture(
						menu->menuTerrainSelect->selectingItemIndex,
						status));
				}
			}break;
			case Scene_BattleField::MapEdit_TroopSelect:{
				spriteIcon.setTexture(game->troopsTextures.getTexture(
					menu->menuTroopSelect->selectingItemIndex));
			}break;
			default:{
				spriteIcon.setTexture(game->mapEditMenuTextures.getTexture(pos));
			}
		}
		//设置文字
		switch(pos){
#define CASE(name) case Scene_BattleField::MapEdit_##name:stringName.setString(#name,true);break;
			BATTLEFIELD_EDIT_MAP_MENU(CASE)
		}
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
SizeType Menu_MapEdit::rowAmount()const{return Scene_BattleField::AmountOfEnumMapEditCommand;}
SizeType Menu_CorpCommand::rowAmount()const{return Campaign::AmountOfCorpEnumMenu;}