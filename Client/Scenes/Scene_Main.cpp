#include"Scene_Main.h"
#include"Game_AdvanceWars.h"
#include"Dialog_NewMap.h"

//插入菜单项
#define INSERT_ITEM(name,menuName)\
menu##menuName.addString(#name,true);

//以下宏用于Scene_Main.h的菜单宏的MACRO参数
#define MAIN_MENU_ITEM(name) INSERT_ITEM(name,Main)
#define SINGLE_MODE_MENU_ITEM(name) INSERT_ITEM(name,SingleMode)
#define ONLINE_MODE_MENU_ITEM(name) INSERT_ITEM(name,OnlineMode)
#define MILITARY_FILES_MENU_ITEM(name) INSERT_ITEM(name,MilitaryFiles)
#define MILITARY_DEPLOY_MENU_ITEM(name) INSERT_ITEM(name,MilitaryDeploy)
#define MILITARY_RECORD_MENU_ITEM(name) INSERT_ITEM(name,MilitaryRecord)
#define GAME_SETTING_MENU_ITEM(name) INSERT_ITEM(name,GameSetting)
#define ABOUT_MENU_ITEM(name) INSERT_ITEM(name,About)

//生成菜单并调整尺寸
#define MAKE_MENU(MENU,name)\
menu##name.setRenderItemAmount(4);\
MENU(MENU##_ITEM)\
menu##name.updateRenderParameters();\
menu##name.onConfirm=::menuConfirm;\
menu##name.onCancel=::menuCancel;

#define GET_GAME auto game=Game_AdvanceWars::currentGame();

//静态变量和函数
static Scene_Main *sceneMain=nullptr;
//事件函数
#define CASE(name) if(menu==&sceneMain->menu##name){sceneMain->menu##name##Confirm();}else
static void menuConfirm(GameMenu *menu){
	CASE(Main)
	MAIN_MENU(CASE){}
}
#undef CASE

static void menuCancel(GameMenu *menu){
	if(menu==&sceneMain->menuMain){
		sceneMain->reset();
	}else{
		sceneMain->removeSubObject(menu);
		sceneMain->addSubObject(&sceneMain->menuMain);
	}
}

Scene_Main::Scene_Main(){
	//生成菜单项
	MAKE_MENU(MAIN_MENU,Main)
	MAKE_MENU(SINGLE_MODE_MENU,SingleMode)
	MAKE_MENU(ONLINE_MODE_MENU,OnlineMode)
	MAKE_MENU(MILITARY_FILES_MENU,MilitaryFiles)
	MAKE_MENU(MILITARY_DEPLOY_MENU,MilitaryDeploy)
	MAKE_MENU(MILITARY_RECORD_MENU,MilitaryRecord)
	MAKE_MENU(GAME_SETTING_MENU,GameSetting)
	MAKE_MENU(ABOUT_MENU,About)
	addSubObject(&menuMain);
	sceneMain=this;
}
Scene_Main::~Scene_Main(){if(sceneMain)sceneMain=nullptr;}

//事件函数
void Scene_Main::menuMainConfirm(){//主菜单确认后,显示各个子菜单
	removeSubObject(&menuMain);
	switch(menuMain.selectingItemIndex){
#define CASE(name) case name:addSubObject(&menu##name);break;
		MAIN_MENU(CASE)
#undef CASE
	}
}

void Scene_Main::menuSingleModeConfirm(){
	GET_GAME
	switch(menuSingleMode.selectingItemIndex){
		case NewMap:{
			auto dialog=game->showDialog_NewMap();
			dialog->setConfirmCallback([game,dialog](){
				//根据对话框的数据创建地图
				dialog->resetBattleField(game->battleField);
				dialog->removeFromParentObject();
				game->battleField.chessPieces.clear();
				//场景跳转
				game->loadAllConfigData();
				auto scene=game->gotoScene_BattleField(true);
				scene->gotoEditMode();//切换到编辑模式
			});
		}break;
		case MapView:{
			auto scene=game->gotoScene_FileList(true);
			scene->selectFile(false,"MapSelect",game->settings.mapsPath,[game](const string &filename){
				game->loadAllConfigData();//加载配置
				if(game->battleField.loadMap_CSV(filename)){//加载地图
					auto scene=game->gotoScene_BattleField(true);//跳转到战场
					scene->gotoEditMode();
				}
			});
		}break;
		case ScenarioMode:{
			auto scene=game->gotoScene_FileList(true);
			scene->selectFile(false,"SelectScript",game->settings.scenarioScriptsPath,[&,game](const string &filename){
				auto script=game->useScenarioScript();
				if(script->executeSenarioScript(filename)){//脚本加载没有问题后,再移除场景
					game->clearAllScenes();//移除场景
					game->loadAllConfigData();
					game->loadAllTextures();
					game->addSubObject(game->useLayerConversation());//添加对话框
				}
			});
		}break;
		//case MissionMode:break;
		case VersusMode:{//打开文件菜单
			auto scene=game->gotoScene_FileList(true);
			scene->selectFile(false,"MapSelect",game->settings.mapsPath,[game](const string &filename){
				game->loadAllConfigData();//加载配置
				if(game->battleField.loadMap_CSV(filename)){//加载地图
					game->loadAllTextures();//加载纹理
					game->gotoScene_CampaignPrepare(true);//进入设置界面
				}
			});
		}break;
		//case SurvivalMode:break;
		//case CombatMode:break;
		default:game->notDone();
	}
}

void Scene_Main::menuOnlineModeConfirm(){
	GET_GAME
	switch(menuOnlineMode.selectingItemIndex){
		case Register:{
			auto dialog=game->showDialog_Login();
			dialog->setIsRegister(menuOnlineMode.selectingItemIndex==Register);
		}break;
		default:game->notDone();
	}
}
void Scene_Main::menuMilitaryFilesConfirm(){
	GET_GAME
	switch(menuMilitaryFiles.selectingItemIndex){
		default:game->notDone();
	}
}
void Scene_Main::menuMilitaryDeployConfirm(){
	GET_GAME
	switch(menuMilitaryDeploy.selectingItemIndex){
		default:game->notDone();
	}
}
void Scene_Main::menuMilitaryRecordConfirm(){
	GET_GAME
	switch(menuMilitaryRecord.selectingItemIndex){
		default:game->notDone();
	}
}
void Scene_Main::menuGameSettingConfirm(){
	GET_GAME
	switch(menuGameSetting.selectingItemIndex){
		default:game->notDone();
	}
}
void Scene_Main::menuAboutConfirm(){
	GET_GAME
	game->notDone();
}