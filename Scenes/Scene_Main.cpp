#include"Scene_Main.h"
#include"Game_AdvanceWars.h"

#include"GameSprite.h"
#include"GameText.h"

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
#define INSERT_ITEM(name,menu)\
menu.insertItem(Game_AdvanceWars::translate(#name));

//以下宏用于Scene_Main.h的菜单宏的MACRO参数
#define MAIN_MENU_ITEM(name) INSERT_ITEM(name,menuMain)
#define SINGLE_MODE_MENU_ITEM(name) INSERT_ITEM(name,menuSingleMode)
#define ONLINE_MODE_MENU_ITEM(name) INSERT_ITEM(name,menuOnlineMode)
#define MILITARY_FILES_MENU_ITEM(name) INSERT_ITEM(name,menuMilitaryFiles)
#define MILITARY_DEPLOY_MENU_ITEM(name) INSERT_ITEM(name,menuMilitaryDeploy)

//生成菜单并调整尺寸
#define MAKE_MENU(MENU_MACRO,menu)\
MENU_MACRO(MENU_MACRO##_ITEM)\
menu.refresh();\
menu.position=Game::resolution/2;

Scene_Main::Scene_Main():status(0),countDown(0){
	//主菜单
	textTitle.setString(Game_AdvanceWars::translate("AdvanceWars_LifeTime"));
	subObjects.push_back(&textTitle);//渲染
	textTitle.position=Game::resolution/2;//放在屏幕中央
	//生成菜单项
	/*MAKE_MENU(MAIN_MENU,menuMain)
	MAKE_MENU(SINGLE_MODE_MENU,menuSingleMode)
	MAKE_MENU(ONLINE_MODE_MENU,menuOnlineMode)
	MAKE_MENU(MILITARY_FILES_MENU,menuMilitaryFiles)
	MAKE_MENU(MILITARY_DEPLOY_MENU,menuMilitaryDeploy)

	currentMenu=&menuMain;*/
}
Scene_Main::~Scene_Main(){}

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
			//subObjects.push_back(&menuMain);//添加主菜单
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