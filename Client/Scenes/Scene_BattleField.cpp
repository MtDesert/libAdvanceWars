#include"Scene_BattleField.h"
#include"Sprite_Unit.h"
#include"Game_AdvanceWars.h"

//人工智能
#include"AI.h"
static AI ai;

Scene_BattleField::Scene_BattleField():battleField(nullptr),campaign(nullptr),
menuMapEdit(nullptr),menuCorpSelect(nullptr),menuTerrainSelect(nullptr),menuTroopSelect(nullptr),
menuCampaign(nullptr),menuProduceSelect(nullptr),menuUnitSelect(nullptr),menuCorpCommand(nullptr),menuCommanderPower(nullptr),
spriteCurrentDay(nullptr),spriteTroopFundsCO(nullptr){
	GAME_AW
	battleField=&game->battleField;
	campaign=&game->campaign;
	//战场图层
	layerBattleField.battleField=battleField;
	addSubObject(&layerBattleField);
	//addSubObject(&layerWeather);
	//layerWeather.setParticleAmount(Game::resolution.x*Game::resolution.y/40,Layer_Weather::StyleLine);
	layerBattleField.whenAnimationUnitMoveOver=[&](){
		if((campaign->selectedUnitData.unit && campaign->selectedUnitData.unit->isWait) || campaign->moveWithPath()){//继续移动,直到完成目标
			printf("命令%d\n",campaign->corpMenuCommand);
			if(campaign->selectedTargetPoint){
				printf("参数%d,%d\n",campaign->selectedTargetPoint->x,campaign->selectedTargetPoint->y);
				campaign->cursor=*campaign->selectedTargetPoint;
				campaign->cursorUnitData.getUnitData(campaign->cursor);
			}
			auto finish=campaign->executeCorpMenu(campaign->corpMenuCommand);
			//行动完毕,下一步
			if(finish){
				nextStep();
			}else{
				printf("命令%d没完没了\n",campaign->corpMenuCommand);
			}
		}
	};
	//菜单按钮
	buttonEdit.position.y=(game->resolution.y-buttonEdit.size.y)/2;
	addSubObject(&buttonEdit);
	addSubObject(&inputLayer);
	//addSubObject(&spriteTerrainInfo);
	//addSubObject(&spriteUnitInfo);

	//时间片
	game->timeSliceList.pushTimeSlice(this,20,40);
}
Scene_BattleField::~Scene_BattleField(){
	GAME_AW
	game->timeSliceList.removeTimeSlice(this);
#define DELETE_MENU(name) if(menu##name)delete menu##name;
	DELETE_MENU(MapEdit)
	DELETE_MENU(CorpSelect)
	DELETE_MENU(TerrainSelect)
	DELETE_MENU(TroopSelect)

	DELETE_MENU(Campaign)
	DELETE_MENU(ProduceSelect)
	DELETE_MENU(UnitSelect)
	DELETE_MENU(CorpCommand)
	DELETE_MENU(CommanderPower)
}

void Scene_BattleField::gotoEditMode(){
	GAME_AW
	layerBattleField.isEditMode=true;
	layerBattleField.isEditMode_Unit=false;
	//创建菜单
	auto &iconsTexs=game->allIconsTextures;
	auto tex=iconsTexs.getTexture("MenuArrow");
#define MAKE_MENU(name) \
	if(!menu##name)menu##name=new Menu_##name();\
	menu##name->pSpriteSelector->setTexture(tex);

	MAKE_MENU(MapEdit)
	MAKE_MENU(CorpSelect)
	MAKE_MENU(TerrainSelect)
	MAKE_MENU(TroopSelect)
	//菜单关系关联
	menuMapEdit->menuCorpSelect=menuCorpSelect;
	menuMapEdit->menuTerrainSelect=menuTerrainSelect;
	menuMapEdit->menuTroopSelect=menuTroopSelect;
	//选择兵种,地形,势力
	auto funcUpdateAllEditMenu=[&](GameMenu *menu){
		menu->removeFromParentObject();
		if(menu==menuTerrainSelect)layerBattleField.isEditMode_Unit=false;
		if(menu==menuCorpSelect)layerBattleField.isEditMode_Unit=true;
		//更新菜单状态
		menuCorpSelect->troopID = menuTerrainSelect->troopID = menuTroopSelect->selectingItemIndex;
		menuCorpSelect->updateRenderParameters(true);
		menuTerrainSelect->updateRenderParameters(true);
		menuMapEdit->updateRenderParameters(true);
	};
	//填充功能
	auto funcFill=[&,game](GameMenu *menu){
		auto code=game->mTerrainCodesList.data(menu->selectingItemIndex);
		if(code){
			battleField->fillTerrain(Terrain(
				menu->selectingItemIndex,
				code->capturable ? menuTroopSelect->selectingItemIndex : 0));
		}
		battleField->autoAdjustTerrainsTiles();
		menu->removeFromParentObject();
	};
	menuMapEdit->onConfirm=[&,game,funcUpdateAllEditMenu,funcFill](GameMenu *menu){
		switch(menu->selectingItemIndex){
			case MapEdit_Intel:{
				auto dialog=game->showDialog_NewMap();
				dialog->setBattleField(*battleField);
				dialog->setConfirmCallback([game,dialog](){
					dialog->resetBattleField(game->battleField);
					dialog->removeFromParentObject();
				});
			}break;
			case MapEdit_CorpSelect:showMenu(*menuCorpSelect,funcUpdateAllEditMenu);break;
			case MapEdit_TerrainSelect:showMenu(*menuTerrainSelect,funcUpdateAllEditMenu);break;
			case MapEdit_TroopSelect:showMenu(*menuTroopSelect,funcUpdateAllEditMenu);break;
			case MapEdit_Fill:showMenu(*menuTerrainSelect,funcFill);//选择地形来填充
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
				Game::dialogConfirm("Exit?",[&](){
					game->gotoScene_Main();
				});
			}break;
			default:;
		}
	};
	//编辑菜单按钮
	buttonEdit.setString("Edit",true);
	buttonEdit.onClicked=[&](){
		showMenu(*menuMapEdit,menuMapEdit->onConfirm);
	};
}
void Scene_BattleField::gotoBattleMode(){
	GAME_AW
	//创建菜单
	auto &iconsTexs=game->allIconsTextures;
	auto tex=iconsTexs.getTexture("MenuArrow");
	MAKE_MENU(Campaign)
	MAKE_MENU(ProduceSelect)
	MAKE_MENU(UnitSelect)
	MAKE_MENU(CorpCommand)
	MAKE_MENU(CommanderPower)
	menuProduceSelect->produceCorpArray = &campaign->produceMenu;
	//各种信息部件
	if(!spriteCurrentDay)spriteCurrentDay=new Sprite_CurrentDay();
	if(!spriteTroopFundsCO)spriteTroopFundsCO=new Sprite_TroopFundsCO();

	layerBattleField.isEditMode = layerBattleField.isEditMode_Unit = false;
	menuCorpCommand->corpCommandArray = &campaign->corpMenu;
	campaign->beginTurn();//回合开始
	//菜单事件
	menuCampaign->onConfirm=[&,game](GameMenu *menu){
		switch(menu->selectingItemIndex){
			case Campaign_Power:{//显示指挥官能力菜单
				auto powers=campaign->currentCOpowersList();
				if(powers){//显示能力信息
					menuCommanderPower->allPowers=powers;
					showMenu(*menuCommanderPower,[&](GameMenu *menu){
						menu->removeFromParentObject();//菜单消失
						campaign->changeCOpowerLevel(menuCommanderPower->selectingItemIndex);//改变能力状态
					});
				}else{//显示无
					Game::dialogOK("No power!");
				}
			}break;
			case Campaign_ChangeCO:{//显示当前CO列表
			}break;
			case Campaign_EndTurn://回合结束
				endTurn();
			break;
			case Campaign_SaveFile:{//保存游戏进度
				auto scene=game->gotoScene_FileList(true);
				scene->selectFile(true,"SaveGame",game->settings.savesPath,[&,scene](const string &filename){
					campaign->saveCampaign(filename);
					scene->buttonCancel.onClicked();
				});
			}break;
			case Campaign_ExitMap:{
				Game::dialogConfirm("Exit?",[&](){
					game->gotoScene_Main(true);
				});
			}break;
			default:game->notDone();
		}
	};
	//战役菜单
	buttonEdit.setString("Menu",true);
	buttonEdit.onClicked=[&](){
		showMenu(*menuCampaign,menuCampaign->onConfirm);
	};
	//回合开始
	beginTurn();
}

static bool aiStartedThink=false;
void Scene_BattleField::beginTurn(){
	allowInput(false);
	//设置需要显示的内容
	campaign->beginTurn();
	if(!spriteCurrentDay)return;
	spriteCurrentDay->setCampaign(*campaign);
	//淡入淡出动画
	reAddSubObject(spriteCurrentDay);
	spriteCurrentDay->color.alpha=0;
	cdFadeTo.fadeTo(spriteCurrentDay,1000,0xFF,[&](){
		cdDelay.startCountDown(2000,[&](){
			cdFadeTo.fadeTo(spriteCurrentDay,1000,0,[&](){//动画结束
				removeSubObject(spriteCurrentDay);
				//并显示部队状况
				reAddSubObject(spriteTroopFundsCO);
				reAddSubObject(&spriteTerrainInfo);
				reAddSubObject(&spriteUnitInfo);
				//位置
				spriteTroopFundsCO->setScreenPosition_Corner(false,true);
				spriteTerrainInfo.setScreenPosition_Corner(false,false);
				spriteUnitInfo.setScreenPosition_Corner(true,false);
				//获得收入
				campaign->currentTroop_GetIncome();
				spriteTroopFundsCO->setCampaignTroop(*campaign->currentTroop());
				//开始行动
				nextStep();
			});
		});
	});
}
void Scene_BattleField::nextStep(){
	auto troop=campaign->currentTroop();
	if(troop->isAI){//开始思考
		aiThink();
	}else{//人脑,允许用户操作
		allowInput(true);
	}
}
void Scene_BattleField::endTurn(){
	menuCampaign->removeFromParentObject();
	campaign->endTurn();
	beginTurn();
}

void Scene_BattleField::reset(){
	//可以在这里加载资源
	GAME_AW
	game->loadAllTextures();
	layerBattleField.updateMapRect();//调整尺寸
	//设置信息显示框的内容
	auto &iconsTexs=game->allIconsTextures;
	spriteTerrainInfo.starIcon.setTexture(iconsTexs.getTexture("Star"));
	spriteUnitInfo.iconHP.setTexture(iconsTexs.getTexture("Heart"));
	spriteUnitInfo.iconGas.setTexture(iconsTexs.getTexture("Supply"));
	spriteUnitInfo.iconAmmo.setTexture(iconsTexs.getTexture("Ammo"));
	spriteUnitInfo.verticalLayout(spriteUnitInfo.size.y/2,0);
}
void Scene_BattleField::consumeTimeSlice(){
	if(aiStartedThink && !ai.isThinking()){//AI思考完毕
		aiStartedThink=false;//防止频繁进入此分支
		printf("AI开始行动\n");
		//得到结果,开始行动
		UnitAI *unitAI=ai.troopAI.getMovingUnitAI();
		if(unitAI){
			printf("单位(%d,%d)开始行动\n",unitAI->unit->coordinate.x,unitAI->unit->coordinate.y);
			printf("目标点%d,%d\n",unitAI->moveCoord.x,unitAI->moveCoord.y);
			//获取移动路径(模仿玩家操作)
			campaign->setCursor(unitAI->unit->coordinate);//选择单位
			campaign->cursorConfirm();//得到移动范围
			//传递AI参数
			campaign->setCursor(unitAI->moveCoord);//移动到目标
			campaign->corpMenuCommand=unitAI->command;//设置命令
			campaign->selectedTargetPoint=&unitAI->commandCoord;
			//先执行移动
			campaign->moveWithPath();
		}else{
			endTurn();
		}
	}
}
void Scene_BattleField::allowInput(bool allow){
	inputLayer.forceIntercept=!allow;
}
void Scene_BattleField::setCursor(const Campaign::CoordType p){
	spriteTerrainInfo.setUnitData(campaign->cursorUnitData);//实时显示地形信息
	spriteUnitInfo.setUnitData(campaign->cursorUnitData);//实时显示单位信息
	//调整信息框位置
}

void Scene_BattleField::aiThink(){
	ai.setCampaign(*campaign);
	aiStartedThink = ai.startThinking();
}

void Scene_BattleField::showMenu(GameMenu &menu,decltype(GameMenu::onConfirm) onConfirm,decltype(GameMenu::onCancel) onCancel){
	menu.onConfirm=onConfirm;
	if(onCancel){//不指定取消函数的情况下,就不要动原来的函数
		menu.onCancel=onCancel;
	}
	reAddSubObject(&menu);
	menu.updateRenderParameters(true);
}
void Scene_BattleField::updateMenu(){
	//生产选择菜单
	if(!menuProduceSelect->parentObject && campaign->produceMenu.size()){//显示
		menuProduceSelect->selectingItemIndex=0;
		menuProduceSelect->troopID=campaign->produceTroopID;//设定生产势力
		showMenu(*menuProduceSelect,[&](GameMenu*){
			campaign->executeProduceMenu(menuProduceSelect->selectingItemIndex);
			updateMenu();
		});
	}
	if(menuProduceSelect->parentObject && campaign->produceMenu.size()<=0){//显示
		removeSubObject(menuProduceSelect);
	}
	//兵种命令菜单
	if(!menuCorpCommand->parentObject && campaign->corpMenu.size()){//显示
		menuCorpCommand->selectingItemIndex=0;
		showMenu(*menuCorpCommand,[&](GameMenu*){
			auto cmd=campaign->corpMenu.data(menuCorpCommand->selectingItemIndex);
			if(cmd){
				campaign->executeCorpMenu(*cmd);
				updateMenu();//菜单消失
			}
		});
	}
	if(menuCorpCommand->parentObject && campaign->corpMenu.size()==0){//消失
		removeSubObject(menuCorpCommand);
	}
	//单位选择菜单
	if(!menuUnitSelect->parentObject && campaign->unitsArray.size()){
		menuUnitSelect->unitArray = &campaign->unitsArray;
		if(campaign->corpMenuCommand==Campaign::Menu_Drop){//卸载命令
			showMenu(*menuUnitSelect,[&](GameMenu*){//选择卸载单位后,选择卸载位置
				auto pUnit = *campaign->unitsArray.data(menuUnitSelect->selectingItemIndex);
				campaign->selectDropPoint(*pUnit);
				updateMenu();//菜单消失
			},[&](GameMenu*){//取消的话,则要清除数据
				campaign->unitsArray.clear();
				menuUnitSelect->removeFromParentObject();
			});
		}
	}
	if(menuUnitSelect->parentObject && campaign->unitsArray.size()==0){
		removeSubObject(menuUnitSelect);
	}
}