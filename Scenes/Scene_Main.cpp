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
menu##menuName.addString(Game_AdvanceWars::currentGame()->translate(#name));

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
menu##name.position=Game::resolution/2;\
menu##name.itemWidth=200;\
menu##name.onConfirm=::menu##name##Confirm;\
menu##name.onCancel=::menu##name##Cancel;\

//静态变量和函数
static Scene_Main *sceneMain=nullptr;
#define STATIC_CALL_MEMBER(functionName)\
static void functionName(){sceneMain->functionName();}\

#define SCENE_MAIN_CONFIRM_CANCEL(name)\
STATIC_CALL_MEMBER(menu##name##Confirm)\
STATIC_CALL_MEMBER(menu##name##Cancel)

//静态函数调用成员函数
SCENE_MAIN_CONFIRM_CANCEL(Main)
MAIN_MENU(SCENE_MAIN_CONFIRM_CANCEL)

Scene_Main::Scene_Main():status(0),countDown(0){
	//主菜单
	textTitle.setString(Game::currentGame()->translate("AdvanceWars_LifeTime"));
	subObjects.push_back(&textTitle);//渲染
	textTitle.position=Game::resolution/2;//放在屏幕中央
	//生成菜单项
	MAKE_MENU(MAIN_MENU,Main)
	MAKE_MENU(SINGLE_MODE_MENU,SingleMode)
	MAKE_MENU(ONLINE_MODE_MENU,OnlineMode)
	MAKE_MENU(MILITARY_FILES_MENU,MilitaryFiles)
	MAKE_MENU(MILITARY_DEPLOY_MENU,MilitaryDeploy)
	MAKE_MENU(MILITARY_RECORD_MENU,MilitaryRecord)
	MAKE_MENU(GAME_SETTING_MENU,GameSetting)
	MAKE_MENU(ABOUT_MENU,About)

	//菜单事件
	//currentMenu=&menuMain;
	sceneMain=this;
}
Scene_Main::~Scene_Main(){
	if(sceneMain)sceneMain=nullptr;
}

void Scene_Main::reset(){
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
			subObjects.remove(&textTitle);//移除文本标题
			//我们可以在这里播个碉堡的音乐
			subObjects.push_back(&menuMain);//添加主菜单
		break;
		default:;
	}
	//状态切换
	if(countDown<=0 && status < StatusOver){
		++status;
		countDown=(status >= StatusOver?0:sliceValue[status]);
	}
	//处理菜单响应
	/*if(status==StatusOver && currentMenu){
		//处理主菜单
		if(currentMenu->menuStatus==GameMenuStatus::Confirm){
			currentMenu->menuStatus=GameMenuStatus::Selecting;
			if(currentMenu==&menuMain){//主菜单事件
				subObjects.remove(currentMenu);
				currentMenu=nullptr;
				//不显示主菜单,显示子菜单
				switch(menuMain.selectingIndex()){
					case SingleMode:currentMenu=&menuSingleMode;break;//单机模式
					case OnlineMode:currentMenu=&menuOnlineMode;break;//网络模式
					case MilitaryFiles:currentMenu=&menuMilitaryFiles;break;//军事资料
					case MilitaryDeploy:currentMenu=&menuMilitaryDeploy;break;//军事部署
					case MilitaryRecord:
					case GameSetting:
					case AboutThisWork:
						currentMenu=&menuMain;
					break;//关于游戏
				}
				if(currentMenu){
					subObjects.push_back(currentMenu);
				}
			}else if(currentMenu==&menuMilitaryFiles){
				//切换场景(加载资料的场景)
				switch(menuMilitaryFiles.selectingIndex()){
					case CorpsFiles:break;
					case CommandersFiles:break;
					case TroopsFiles:break;
					case TerrainsFiles:break;
					case WeathersFiles:break;
				}
				Game_AdvanceWars::currentGame()->gotoScene_FileData((Game_AdvanceWars::FileType)menuMilitaryFiles.selectingIndex(),"");
			}else if(currentMenu==&menuMilitaryDeploy){
				if(currentMenu->selectingIndex()==0){//加载战场文件
					//Game_AdvanceWars::currentGame()->gotoScene_FileList(Game_AdvanceWars::File_BattleField);
				}
			}
		}else if(currentMenu->menuStatus==GameMenuStatus::Cancel){
			currentMenu->menuStatus=GameMenuStatus::Selecting;
			if(currentMenu==&menuMain){//暂时没有动作
			}else{
				subObjects.remove(currentMenu);
				subObjects.push_back(&menuMain);
				currentMenu=&menuMain;
			}
		}
	}*/
}

//事件函数
void Scene_Main::menuMainConfirm(){}
void Scene_Main::menuMainCancel(){}
void Scene_Main::menuSingleModeConfirm(){}
void Scene_Main::menuSingleModeCancel(){}
void Scene_Main::menuOnlineModeConfirm(){}
void Scene_Main::menuOnlineModeCancel(){}
void Scene_Main::menuMilitaryFilesConfirm(){}
void Scene_Main::menuMilitaryFilesCancel(){}
void Scene_Main::menuMilitaryDeployConfirm(){}
void Scene_Main::menuMilitaryDeployCancel(){}
void Scene_Main::menuMilitaryRecordConfirm(){}
void Scene_Main::menuMilitaryRecordCancel(){}
void Scene_Main::menuGameSettingConfirm(){}
void Scene_Main::menuGameSettingCancel(){}
void Scene_Main::menuAboutConfirm(){}
void Scene_Main::menuAboutCancel(){}