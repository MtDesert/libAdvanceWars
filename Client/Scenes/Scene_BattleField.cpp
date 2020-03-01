#include"Scene_BattleField.h"
#include"Sprite_Unit.h"
#include"Game_AdvanceWars.h"

#include"extern.h"

//渲染变量
static Game_AdvanceWars *game=nullptr;

Scene_BattleField::Scene_BattleField():battleField(nullptr){
	game=Game_AdvanceWars::currentGame();
	battleField=&game->battleField;
	campaign=&game->campaign;
	//战场图层
	layerBattleField.battleField=battleField;
	addSubObject(&layerBattleField);
}
Scene_BattleField::~Scene_BattleField(){}

void Scene_BattleField::gotoEditMode(){
	layerBattleField.isEditMode=true;
	layerBattleField.isEditMode_Unit=false;
	//菜单关系关联
	menuCorpSelect.menuTroopSelect=&menuTroopSelect;
	menuTerrainSelect.menuTroopSelect=&menuTroopSelect;
	menuMapEdit.menuCorpSelect=&menuCorpSelect;
	menuMapEdit.menuTerrainSelect=&menuTerrainSelect;
	menuMapEdit.menuTroopSelect=&menuTroopSelect;
	//选择兵种,地形,势力
	auto funcUpdateAllEditMenu=[&](GameMenu *menu){
		menu->removeFromParentObject();
		if(menu==&menuTerrainSelect)layerBattleField.isEditMode_Unit=false;
		if(menu==&menuCorpSelect)layerBattleField.isEditMode_Unit=true;
		//更新菜单状态
		menuCorpSelect.updateRenderParameters(true);
		menuTerrainSelect.updateRenderParameters(true);
		menuMapEdit.updateRenderParameters(true);
	};
	//填充功能
	auto funcFill=[&](GameMenu *menu){
		auto code=game->mTerrainCodesList.data(menu->selectingItemIndex);
		if(code){
			battleField->fillTerrain(Terrain(
				menu->selectingItemIndex,
				code->capturable ? menuTroopSelect.selectingItemIndex : 0));
		}
		battleField->autoAdjustTerrainsTiles();
		menu->removeFromParentObject();
	};
	//
	menuMapEdit.onConfirm=[&,funcUpdateAllEditMenu,funcFill](GameMenu *menu){
		switch(menu->selectingItemIndex){
			case MapEdit_Intel:{
				auto dialog=game->showDialog_NewMap();
				dialog->setBattleField(*battleField);
			}break;
			case MapEdit_CorpSelect:showMenu(menuCorpSelect,funcUpdateAllEditMenu);break;
			case MapEdit_TerrainSelect:showMenu(menuTerrainSelect,funcUpdateAllEditMenu);break;
			case MapEdit_TroopSelect:showMenu(menuTroopSelect,funcUpdateAllEditMenu);break;
			case MapEdit_Fill:
				showMenu(menuTerrainSelect,funcFill);//选择地形来填充
			break;
			case MapEdit_LoadFile:break;
			case MapEdit_SaveFile:{
				auto scene=game->gotoScene_FileList(true);
				scene->setSaveMode(true);
				scene->lastScene=this;
				scene->stringTitle.setString("SaveMap",true);
				scene->changeDirectory(game->settings.mapsPath);
				scene->whenConfirmFile=[&,scene](const string &filename){
					battleField->saveMap_CSV(filename);
					scene->buttonCancel.onClicked();
				};
			}break;
			case MapEdit_ExitMap:
				game->gotoScene_Main();
			break;
			default:;
		}
	};
	//编辑菜单按钮
	buttonEdit.setString("Edit",true);
	buttonEdit.position.y=(game->resolution.y-buttonEdit.size.y)/2;
	addSubObject(&buttonEdit);
	buttonEdit.onClicked=[&](){
		showMenu(menuMapEdit,menuMapEdit.onConfirm);
	};
}

void Scene_BattleField::reset(){
	//可以在这里加载资源
	game->loadAllTextures();
	layerBattleField.updateMapRect();//调整尺寸
	//菜单样式
#define MENU_INIT(Name) \
menu##Name.bgColor = &ColorRGBA::Black;\
menu##Name.borderColor = &ColorRGBA::White;\
menu##Name.pSpriteSelector->setTexture(game->texMenuArrow);

	MENU_INIT(CorpSelect)
	MENU_INIT(TerrainSelect)
	MENU_INIT(TroopSelect)
	MENU_INIT(MapEdit)
	MENU_INIT(CorpCommand)
}
bool Scene_BattleField::keyboardKey(Keyboard::KeyboardKey key,bool pressed){return false;}
bool Scene_BattleField::mouseKey(MouseKey key,bool pressed){return false;}

void Scene_BattleField::showMenu(GameMenu &menu,decltype(GameMenu::onConfirm) onConfirm){
	menu.onConfirm=onConfirm;
	addSubObject(&menu);
	menu.updateRenderParameters(true);
}
void Scene_BattleField::updateMenu(){
	if(!menuCorpCommand.parentObject && campaign->corpMenu.size()){
		showMenu(menuCorpCommand,nullptr);
	}
	if(menuCorpCommand.parentObject && campaign->corpMenu.size()==0){
		removeSubObject(&menuCorpCommand);
	}
}