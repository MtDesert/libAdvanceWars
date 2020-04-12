#include"Scene_BattleField.h"
#include"Sprite_Unit.h"
#include"Game_AdvanceWars.h"

Scene_BattleField::Scene_BattleField():battleField(nullptr){
	GAME_AW
	battleField=&game->battleField;
	campaign=&game->campaign;
	//战场图层
	layerBattleField.battleField=battleField;
	addSubObject(&layerBattleField);
	//addSubObject(&spriteTerrainInfo);
	//addSubObject(&spriteUnitInfo);
}
Scene_BattleField::~Scene_BattleField(){}

void Scene_BattleField::gotoEditMode(){
	GAME_AW
	layerBattleField.isEditMode=true;
	layerBattleField.isEditMode_Unit=false;
	//菜单关系关联
	menuMapEdit.menuCorpSelect=&menuCorpSelect;
	menuMapEdit.menuTerrainSelect=&menuTerrainSelect;
	menuMapEdit.menuTroopSelect=&menuTroopSelect;
	//选择兵种,地形,势力
	auto funcUpdateAllEditMenu=[&](GameMenu *menu){
		menu->removeFromParentObject();
		if(menu==&menuTerrainSelect)layerBattleField.isEditMode_Unit=false;
		if(menu==&menuCorpSelect)layerBattleField.isEditMode_Unit=true;
		//更新菜单状态
		menuCorpSelect.troopID = menuTerrainSelect.troopID = menuTroopSelect.selectingItemIndex;
		menuCorpSelect.updateRenderParameters(true);
		menuTerrainSelect.updateRenderParameters(true);
		menuMapEdit.updateRenderParameters(true);
	};
	//填充功能
	auto funcFill=[&,game](GameMenu *menu){
		auto code=game->mTerrainCodesList.data(menu->selectingItemIndex);
		if(code){
			battleField->fillTerrain(Terrain(
				menu->selectingItemIndex,
				code->capturable ? menuTroopSelect.selectingItemIndex : 0));
		}
		battleField->autoAdjustTerrainsTiles();
		menu->removeFromParentObject();
	};
	menuMapEdit.onConfirm=[&,game,funcUpdateAllEditMenu,funcFill](GameMenu *menu){
		switch(menu->selectingItemIndex){
			case MapEdit_Intel:{
				auto dialog=game->showDialog_NewMap();
				dialog->setBattleField(*battleField);
				dialog->setConfirmCallback([game,dialog](){
					dialog->resetBattleField(game->battleField);
					dialog->removeFromParentObject();
				});
			}break;
			case MapEdit_CorpSelect:showMenu(menuCorpSelect,funcUpdateAllEditMenu);break;
			case MapEdit_TerrainSelect:showMenu(menuTerrainSelect,funcUpdateAllEditMenu);break;
			case MapEdit_TroopSelect:showMenu(menuTroopSelect,funcUpdateAllEditMenu);break;
			case MapEdit_Fill:showMenu(menuTerrainSelect,funcFill);//选择地形来填充
			break;
			case MapEdit_LoadFile:{
				auto scene=game->gotoScene_FileList(true);
				scene->selectFile(false,"LoadMap",game->settings.mapsPath,[&,scene](const string &filename){
					battleField->loadMap_CSV(filename);
					scene->buttonCancel.onClicked();
				});
			}break;
			case MapEdit_SaveFile:{
				auto scene=game->gotoScene_FileList(true);
				scene->selectFile(true,"SaveMap",game->settings.mapsPath,[&,scene](const string &filename){
					battleField->saveMap_CSV(filename);
					scene->buttonCancel.onClicked();
				});
			}break;
			case MapEdit_ExitMap:{
				auto dialog=game->showDialog_Message();
				dialog->setText("Exit?");
				dialog->setConfirmCallback([&,dialog](){
					dialog->removeFromParentObject();
					game->gotoScene_Main();
				});
			}break;
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
void Scene_BattleField::gotoBattleMode(){
	layerBattleField.isEditMode = layerBattleField.isEditMode_Unit = false;
	menuCorpCommand.corpCommandArray = &campaign->corpMenu;
	campaign->beginTurn();//回合开始
}

void Scene_BattleField::reset(){
	//可以在这里加载资源
	GAME_AW
	game->loadAllTextures();
	layerBattleField.updateMapRect();//调整尺寸
	//菜单样式
	auto &iconsTexs=game->allIconsTextures;
	auto tex=iconsTexs.getTexture("MenuArrow");
#define MENU_INIT(Name) menu##Name.pSpriteSelector->setTexture(tex);
	MENU_INIT(CorpSelect)
	MENU_INIT(TerrainSelect)
	MENU_INIT(TroopSelect)
	MENU_INIT(MapEdit)
	MENU_INIT(UnitSelect)
	MENU_INIT(CorpCommand)
	//设置信息显示框的内容
	spriteTerrainInfo.starIcon.setTexture(iconsTexs.getTexture("Star"));
	spriteUnitInfo.iconHP.setTexture(iconsTexs.getTexture("Heart"));
	spriteUnitInfo.iconGas.setTexture(iconsTexs.getTexture("Supply"));
	spriteUnitInfo.iconAmmo.setTexture(iconsTexs.getTexture("Ammo"));
	spriteUnitInfo.verticalLayout(spriteUnitInfo.size.y/2,0);
}
void Scene_BattleField::setCursor(const Campaign::CoordType p){
	spriteTerrainInfo.setUnitData(campaign->cursorUnitData);//实时显示地形信息
	spriteUnitInfo.setUnitData(campaign->cursorUnitData);//实时显示单位信息
	//调整信息框位置

}

void Scene_BattleField::showMenu(GameMenu &menu,decltype(GameMenu::onConfirm) onConfirm,decltype(GameMenu::onCancel) onCancel){
	menu.onConfirm=onConfirm;
	if(onCancel){//不指定取消函数的情况下,就不要动原来的函数
		menu.onCancel=onCancel;
	}
	addSubObject(&menu);
	menu.updateRenderParameters(true);
}
void Scene_BattleField::updateMenu(){
	//兵种命令菜单
	if(!menuCorpCommand.parentObject && campaign->corpMenu.size()){//显示
		menuCorpCommand.selectingItemIndex=0;
		showMenu(menuCorpCommand,[&](GameMenu*){
			auto cmd=campaign->corpMenu.data(menuCorpCommand.selectingItemIndex);
			if(cmd){
				campaign->executeCorpMenu(*cmd);
				updateMenu();//菜单消失
			}
		});
	}
	if(menuCorpCommand.parentObject && campaign->corpMenu.size()==0){//消失
		removeSubObject(&menuCorpCommand);
	}
	//单位选择菜单
	if(!menuUnitSelect.parentObject && campaign->unitsArray.size()){
		menuUnitSelect.unitArray = &campaign->unitsArray;
		if(campaign->corpMenuCommand==Campaign::Menu_Drop){//卸载命令
			showMenu(menuUnitSelect,[&](GameMenu*){//选择卸载单位后,选择卸载位置
				auto pUnit = *campaign->unitsArray.data(menuUnitSelect.selectingItemIndex);
				campaign->selectDropPoint(*pUnit);
				updateMenu();//菜单们消失
			},[&](GameMenu*){//取消的话,则要清除数据
				campaign->unitsArray.clear();
				menuUnitSelect.removeFromParentObject();
			});
		}
	}
	if(menuUnitSelect.parentObject && campaign->unitsArray.size()==0){
		removeSubObject(&menuUnitSelect);
	}
}