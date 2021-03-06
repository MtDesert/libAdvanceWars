#ifndef GAMESCENE_MAIN_H
#define GAMESCENE_MAIN_H

#include"GameScene.h"
#include"GameMenu_String.h"

//主菜单
#define MAIN_MENU(MACRO)\
MACRO(SingleMode)\
MACRO(OnlineMode)\
MACRO(MilitaryFiles)\
MACRO(MilitaryDeploy)\
MACRO(MilitaryRecord)\
MACRO(GameSetting)\
MACRO(About)

//单机模式菜单
#define SINGLE_MODE_MENU(MACRO)\
MACRO(NewMap)\
MACRO(MapView)\
MACRO(ScenarioMode)\
MACRO(MissionMode)\
MACRO(VersusMode)\
MACRO(SurvivalMode)\
MACRO(CombatMode)

//联机模式菜单
#define ONLINE_MODE_MENU(MACRO)\
MACRO(Register)\
MACRO(Login)\
MACRO(PlayerInfo)\
MACRO(LeaveMessage)\
MACRO(ViewOnlinePlayer)\
MACRO(MakeWar)\
MACRO(JoinWar)\
MACRO(DeclareWar)\
MACRO(AcceptWar)\
MACRO(WatchWar)\
MACRO(League)\
MACRO(MyTurnWars)

//军事资料菜单
#define MILITARY_FILES_MENU(MACRO)\
MACRO(CorpsFiles)\
MACRO(CommandersFiles)\
MACRO(TroopsFiles)\
MACRO(TerrainsFiles)\
MACRO(WeathersFiles)

//军事部署
#define MILITARY_DEPLOY_MENU(MACRO)\
MACRO(DesignMaps)\
MACRO(DesignCommanders)

//军事记录
#define MILITARY_RECORD_MENU(MACRO)\
MACRO(WinLose)\
MACRO(UnitUse)
//游戏设置
#define GAME_SETTING_MENU(MACRO)\
MACRO(CommonSettings)\
MACRO(SpecialSettings)
//关于
#define ABOUT_MENU(MACRO)\
MACRO(ThisGame)\
MACRO(Developer)\
MACRO(DevelopEnv)

//主场景,显示一下简短信息,然后播放一下动画,然后进入主界面菜单显示
class Scene_Main:public GameScene{
public:
	Scene_Main();
	~Scene_Main();

	//菜单项
#undef ENUM
#define ENUM(name) name,
	enum Enum_MainMenu{
		MAIN_MENU(ENUM)
		AmountOf_MainMenu
	};
	enum Enum_SingleMode{
		SINGLE_MODE_MENU(ENUM)
		AmountOf_SingleMode
	};
	enum Enum_OnlineMode{
		ONLINE_MODE_MENU(ENUM)
		AmountOf_OnlineMode
	};
	enum Enum_MilitaryFiles{
		MILITARY_FILES_MENU(ENUM)
		AmountOf_MilitaryFiles
	};
	enum Enum_MilitaryDeploy{
		MILITARY_DEPLOY_MENU(ENUM)
		AmountOf_MilitaryDeploy
	};
	enum Enum_MilitaryRecord{
		AmountOf_MilitaryRecord
	};
	enum Enum_Setting{
		AmountOf_Setting
	};
	enum Enum_About{
		AmountOf_About
	};
#undef ENUM

//菜单声明
#define MENU_DECLARE(name)\
	GameMenu_String menu##name;/*菜单对象*/\
	void menu##name##Confirm();/*菜单确认函数*/

	MENU_DECLARE(Main)//主菜单
	MAIN_MENU(MENU_DECLARE)//所有子菜单
#undef MENU_DECLARE
};
#endif