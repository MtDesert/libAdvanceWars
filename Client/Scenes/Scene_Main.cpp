#include"Scene_Main.h"
#include"Game_AdvanceWars.h"

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
MENU(MENU##_ITEM)\
menu##name.itemWidth=200;\
menu##name.updateRenderParameters();\
menu##name.onConfirm=::menuConfirm;\
menu##name.onCancel=::menuCancel;

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
	menuMain.renderItemAmount=7;
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
static void whenSingleSenarioMode(const string &filename){
	auto game=Game_AdvanceWars::currentGame();
	auto script=game->useScenarioScript();
	if(script->executeSenarioScript(filename)){//脚本加载没有问题后,再移除场景
		game->clearAllScenes();//移除场景
		game->addSubObject(game->useLayerConversation());//添加对话框
	}
}
//对战模式选择地图后
static void whenSingleVersusSelectedFile(const string &filename){
	//Game_AdvanceWars::currentGame()->gotoScene_BattleField(filename);
}

void Scene_Main::menuSingleModeConfirm(){
	auto game=Game_AdvanceWars::currentGame();
	switch(menuSingleMode.selectingItemIndex){
		case ScenarioMode:{
			auto scene=game->gotoScene_FileList();
			scene->textTitle.setString("SelectScript",true);
			scene->changeDirectory(game->settings.scenarioScriptsPath);
			scene->whenConfirmFile=whenSingleSenarioMode;
		}break;
		case MissionMode:break;
		case VersusMode:{//打开文件菜单
			auto scene=game->gotoScene_FileList();
			scene->textTitle.setString("SelectMap",true);
			scene->changeDirectory(game->settings.mapsPath);
			scene->whenConfirmFile=whenSingleVersusSelectedFile;
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