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
	menuMain.setRenderItemAmount(7);
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

//单机-剧情模式选择剧本后
/*static void whenSingleMode_Scenario(const string &filename){
	GET_GAME
	auto script=game->useScenarioScript();
	if(script->executeSenarioScript(filename)){//脚本加载没有问题后,再移除场景
		game->clearAllScenes();//移除场景
		game->addSubObject(game->useLayerConversation());//添加对话框
	}
}
//对战模式选择地图后
static void whenSingleVersusSelectedFile(const string &filename){}*/

void Scene_Main::menuSingleModeConfirm(){
	GET_GAME
	switch(menuSingleMode.selectingItemIndex){
		case NewMap:{
			auto dialog=new Dialog_NewMap();
			addSubObject(dialog);
		}break;
		case MapView:{
			auto scene=game->gotoScene_FileList();
			scene->textTitle.setString("MapSelect",true);
			scene->changeDirectory(game->settings.mapsPath);
			scene->whenConfirmFile=[game](const string &filename){
				//加载资源
				game->loadAllConfigData();//加载配置
				game->loadAllTextures();//加载纹理
				if(game->battleField.loadMap_CSV(filename)){//加载地图
					auto scene=game->gotoScene_BattleField();//跳转到战场
					scene->updateMapRect();
				}
			};
		}break;
		case ScenarioMode:{
			/*scene->textTitle.setString("SelectScript",true);
			scene->changeDirectory(game->settings.scenarioScriptsPath);
			scene->whenConfirmFile=whenSingleMode_Scenario;*/
		}break;
		case MissionMode:break;
		case VersusMode:{//打开文件菜单
			/*scene->textTitle.setString("SelectMap",true);
			scene->changeDirectory(game->settings.mapsPath);
			scene->whenConfirmFile=whenSingleVersusSelectedFile;*/
		}break;
		case SurvivalMode:break;
		case CombatMode:break;
	}
}

void Scene_Main::menuOnlineModeConfirm(){
	switch(menuOnlineMode.selectingItemIndex){
		case Register:
			showLoginDialog(true);
		break;
		case Login:
			showLoginDialog();
		break;
	}
}
void Scene_Main::menuMilitaryFilesConfirm(){}
void Scene_Main::menuMilitaryDeployConfirm(){}
void Scene_Main::menuMilitaryRecordConfirm(){}
void Scene_Main::menuGameSettingConfirm(){}
void Scene_Main::menuAboutConfirm(){}