#include"Scene_Main.h"
#include"Game_AdvanceWars.h"

//流程控制
enum Status{
	FadeIn,Delay,FadeOut,
	ShowMainMenu,
	StatusOver
};

static int sliceValue[StatusOver]={
	1000000,
	2000000,
	1000000,
	0
};

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
menu##name.onConfirm=::menu##name##Confirm;\
menu##name.onCancel=::menu##name##Cancel;\
menu##name.updateRenderParameters();

//静态变量和函数
static Scene_Main *sceneMain=nullptr;
#define STATIC_CALL_MEMBER(functionName)\
static void functionName(){sceneMain->functionName();}\

#define SCENEMAIN_CONFIRM_CANCEL(name)\
STATIC_CALL_MEMBER(menu##name##Confirm)\
STATIC_CALL_MEMBER(menu##name##Cancel)

//静态函数调用成员函数
SCENEMAIN_CONFIRM_CANCEL(Main)
MAIN_MENU(SCENEMAIN_CONFIRM_CANCEL)

Scene_Main::Scene_Main():status(0),countDown(0){
	//主菜单
	textTitle.setString("AdvanceWars_LifeTime",true);
	addSubObject(&textTitle);//渲染
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

	//菜单事件
	sceneMain=this;
}
Scene_Main::~Scene_Main(){
	if(sceneMain)sceneMain=nullptr;
}

void Scene_Main::reset(){
	status=FadeIn;
	countDown=sliceValue[FadeIn];//以完全透明出场
}

void Scene_Main::consumeTimeSlice(){
	countDown-=minTimeSlice;
	countDown=max(0,countDown);//保证countDown>=0
	//实时执行的代码
	switch(status){
		case FadeIn://1秒显示内容
			textTitle.color.alpha=(sliceValue[status]-countDown)*255/sliceValue[status];
		break;
		case Delay:break;//2秒停顿
		case FadeOut://1秒消失
			textTitle.color.alpha=countDown*255/sliceValue[status];
		break;
		case ShowMainMenu:
			removeSubObject(&textTitle);//移除文本标题
			//我们可以在这里播个碉堡的音乐
			addSubObject(&menuMain);//添加主菜单
		break;
		default:;
	}
	//状态切换
	if(countDown<=0 && status < StatusOver){
		++status;
		countDown=(status >= StatusOver?0:sliceValue[status]);
	}
}

//事件函数
void Scene_Main::menuMainConfirm(){//主菜单确认后,显示各个子菜单
	removeSubObject(&menuMain);
	switch(menuMain.selectingItemIndex){
#define CASE(name) case name:addSubObject(&menu##name);break;
		MAIN_MENU(CASE)
#undef CASE
	}
}

//子菜单取消后,显示主菜单
#define SCENEMAIN_SUBMENU_CANCEL(name)\
void Scene_Main::menu##name##Cancel(){\
	removeSubObject(&menu##name);\
	addSubObject(&menuMain);\
}
MAIN_MENU(SCENEMAIN_SUBMENU_CANCEL)

//单机-剧情模式选择剧本后
static void whenSingleSenarioMode(const string &filename){
	auto game=Game_AdvanceWars::currentGame();
	//清理部分内容
	game->loadSenarioScript(filename);
}
//对战模式选择地图后
static void whenSingleVersusSelectedFile(const string &filename){
	Game_AdvanceWars::currentGame()->gotoScene_BattleField(filename);
}

void Scene_Main::menuMainCancel(){reset();}
void Scene_Main::menuSingleModeConfirm(){
	auto game=Game_AdvanceWars::currentGame();
	switch(menuSingleMode.selectingItemIndex){
		case ScenarioMode:{
			auto scene=game->showScene_FileList();
			scene->textTitle.setString("SelectScript",true);
			scene->changeDirectory(game->settings.senarioScriptsPath);
			scene->whenConfirmFile=whenSingleSenarioMode;
		}break;
		case MissionMode:break;
		case VersusMode:{//打开文件菜单
			auto scene=game->showScene_FileList();
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