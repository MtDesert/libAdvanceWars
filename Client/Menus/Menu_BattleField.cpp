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
	//个别地形比较高,要调整锚点
	spriteIcon.anchorPoint.y=0;
	spriteIcon.position.y=-ICON_SIZE/2;
}
MenuItem_TroopSelect::MenuItem_TroopSelect(){
	size.setXY(ICON_SIZE*8,ICON_SIZE);
	setCursorWidth(ICON_SIZE);
}
MenuItem_CommanderSelect::MenuItem_CommanderSelect(){
	size.setXY(ICON_SIZE*10,ICON_SIZE*2);
	spriteIcon.size.setXY(ICON_SIZE*2,ICON_SIZE*2);
	setCursorWidth(ICON_SIZE);
	stringName.position.y=ICON_SIZE/2;
	//部队图标
	addSubObject(&troopIcon);
	troopIcon.size.setXY(ICON_SIZE,ICON_SIZE);
	troopIcon.position.setXY(spriteIcon.position.x + ICON_SIZE*2.5,-ICON_SIZE/2);
	//部队文字
	troopIcon.addSubObject(&troopName);
	troopName.anchorPoint.setXY(0,0.5);
	troopName.position.setXY(ICON_SIZE/2,0);
}
MenuItem_MapEdit::MenuItem_MapEdit(){
	size.setXY(ICON_SIZE*6,ICON_SIZE);
	setCursorWidth(ICON_SIZE);
}

MenuItem_Campaign::MenuItem_Campaign(){
	size.setXY(ICON_SIZE*6,ICON_SIZE);
	setCursorWidth(ICON_SIZE);
}
MenuItem_ProduceSelect::MenuItem_ProduceSelect(){
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
MenuItem_UnitSelect::MenuItem_UnitSelect(){
	size.setXY(ICON_SIZE*12,ICON_SIZE);
	setCursorWidth(ICON_SIZE);
}
MenuItem_CorpCommand::MenuItem_CorpCommand(){
	size.setXY(ICON_SIZE*4,ICON_SIZE);
	setCursorWidth(ICON_SIZE);
}
MenuItem_CommanderPower::MenuItem_CommanderPower(){
	size.setXY(ICON_SIZE*8,ICON_SIZE);
	setCursorWidth(ICON_SIZE);
}

//Menu们的构造函数
Menu_CorpSelect::Menu_CorpSelect():troopID(0){MENU_INIT}
Menu_TerrainSelect::Menu_TerrainSelect():troopID(0){MENU_INIT}
Menu_TroopSelect::Menu_TroopSelect(){MENU_INIT}
Menu_CommanderSelect::Menu_CommanderSelect(){MENU_INIT}
Menu_MapEdit::Menu_MapEdit():menuCorpSelect(nullptr),menuTerrainSelect(nullptr),menuTroopSelect(nullptr){MENU_INIT}
Menu_Campaign::Menu_Campaign(){MENU_INIT}
Menu_ProduceSelect::Menu_ProduceSelect():troopID(0),produceCorpArray(nullptr){MENU_INIT}
Menu_UnitSelect::Menu_UnitSelect(){MENU_INIT}
Menu_CorpCommand::Menu_CorpCommand():corpCommandArray(nullptr){MENU_INIT}
Menu_CommanderPower::Menu_CommanderPower():allPowers(nullptr){MENU_INIT}

//Item们的updateData函数
void MenuItem_CorpSelect::updateData(SizeType pos){
	auto corp=game->mCorpsList.data(pos);//取数据
	if(corp){
		stringName.setString(corp->translate);
		stringPrice.setString(Number::toString(corp->price));
		auto menu=dynamic_cast<Menu_CorpSelect*>(parentObject);
		if(menu){
			spriteIcon.setTexture(game->corpsIconsArray.getTexture(pos,menu->troopID));
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
			spriteIcon.setTexture(game->terrainsTexturesArray.getTexture(pos,terrainCode->capturable ? menu->troopID : 0));
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
void MenuItem_CommanderSelect::updateData(SizeType pos){
	auto co=game->mCommandersList.data(pos);//取数据
	if(co){
		stringName.setString(co->name);
		spriteIcon.setTexture(game->commandersHeadTextures.getTexture(pos));
		//显示部队信息
		SizeType posA;
		auto troop=game->mTroopsList.dataName(co->troop,posA);
		if(troop){
			troopIcon.setTexture(game->troopsTextures.getTexture(posA));
			troopName.setString(troop->translate);
		}
	}else{
		ITEM_ICON_NAME_NULL
	}
}

void MenuItem_MapEdit::updateData(SizeType pos){
	auto menu=dynamic_cast<Menu_MapEdit*>(parentObject);
	if(menu && pos< Scene_BattleField::AmountOfEnumMapEditCommand){
		switch(pos){
			case Scene_BattleField::MapEdit_CorpSelect:
				spriteIcon.setTexture(game->corpsIconsArray.getTexture(menu->menuCorpSelect->selectingItemIndex,menu->menuCorpSelect->troopID));
			break;
			case Scene_BattleField::MapEdit_TerrainSelect:{
				auto terrainCode=game->mTerrainCodesList.data(menu->menuTerrainSelect->selectingItemIndex);
				if(terrainCode){
					spriteIcon.setTexture(game->terrainsTexturesArray.getTexture(menu->menuTerrainSelect->selectingItemIndex,terrainCode->capturable ? menu->menuTerrainSelect->troopID : 0));
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
#undef CASE
		}
	}else{
		ITEM_ICON_NAME_NULL
	}
}
void MenuItem_Campaign::updateData(SizeType pos){
	auto menu=dynamic_cast<Menu_Campaign*>(parentObject);
	if(menu && pos< Scene_BattleField::AmountOfEnumCampaignCommand){
		spriteIcon.setTexture(game->campaignMenuTextures.getTexture(pos));
		//设置文字
		switch(pos){
#define CASE(name) case Scene_BattleField::Campaign_##name:stringName.setString(#name,true);break;
			BATTLEFIELD_CAMPAIGN_MENU(CASE)
#undef CASE
		}
	}else{
		ITEM_ICON_NAME_NULL
	}
}
void MenuItem_ProduceSelect::updateData(SizeType pos){
	auto menu=dynamic_cast<Menu_ProduceSelect*>(parentObject);
	if(!menu && !menu->produceCorpArray)return;
	auto corpID = menu->produceCorpArray->data(pos);
	if(!corpID)return;
	//显示兵种信息
	auto corp=game->mCorpsList.data(*corpID);
	if(corp){
		stringName.setString(corp->translate);
		stringPrice.setString(Number::toString(corp->price));
		spriteIcon.setTexture(game->corpsIconsArray.getTexture(*corpID,menu->troopID));
	}else{
		ITEM_ICON_NAME_NULL
	}
}
void MenuItem_UnitSelect::updateData(SizeType pos){
	auto menu=dynamic_cast<Menu_UnitSelect*>(parentObject);
	if(!menu && !menu->unitArray)return;
	auto pUnit = menu->unitArray->data(pos);
	if(pUnit && *pUnit){
		auto unit=*pUnit;
		spriteIcon.setTexture(game->corpsIconsArray.getTexture(unit->corpType,unit->color));
	}else{
		ITEM_ICON_NAME_NULL
	}
}

void MenuItem_CorpCommand::updateData(SizeType pos){
	auto menu=dynamic_cast<Menu_CorpCommand*>(parentObject);
	if(!menu && !menu->corpCommandArray)return;
	auto command=menu->corpCommandArray->data(pos);
	//显示指令
	if(command){
		spriteIcon.setTexture(game->corpMenuTextures.getTexture(*command));
		switch(*command){
#define CASE(name) case Campaign::Menu_##name:stringName.setString(#name,true);break;
			CAMPAIGN_CORPMENU(CASE)
#undef CASE
			default:stringName.setString("???");
		}
	}else{
		ITEM_ICON_NAME_NULL
	}
}
void MenuItem_CommanderPower::updateData(SizeType pos){
	auto menu=dynamic_cast<Menu_CommanderPower*>(parentObject);
	if(!menu || !menu->allPowers)return;
	auto power=menu->allPowers->data(pos);
	//显示能力
	if(power){
		//生成图标名字
		char name[10];
		sprintf(name,"Power");
		if(pos>0)sprintf(&name[5],"%lu",pos);
		spriteIcon.setTexture(game->allIconsTextures.getTexture(name));
		stringName.setString(power->translate);
	}else{
		ITEM_ICON_NAME_NULL
	}
}

//Menu们的itemAmount函数
SizeType Menu_CorpSelect::itemAmount()const{return game->mCorpsList.size();}
SizeType Menu_TerrainSelect::itemAmount()const{return game->mTerrainCodesList.size();}
SizeType Menu_TroopSelect::itemAmount()const{return game->mTroopsList.size();}
SizeType Menu_CommanderSelect::itemAmount()const{return game->mCommandersList.size();}
SizeType Menu_MapEdit::itemAmount()const{return Scene_BattleField::AmountOfEnumMapEditCommand;}
SizeType Menu_Campaign::itemAmount()const{return Scene_BattleField::AmountOfEnumCampaignCommand;}
SizeType Menu_ProduceSelect::itemAmount()const{return produceCorpArray ? produceCorpArray->size() : 0;}
SizeType Menu_UnitSelect::itemAmount()const{return unitArray ? unitArray->size() : 0;}
SizeType Menu_CorpCommand::itemAmount()const{return corpCommandArray ? corpCommandArray->size() : 0;}
SizeType Menu_CommanderPower::itemAmount()const{return allPowers ? allPowers->size() : 0;}