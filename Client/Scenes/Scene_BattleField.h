#ifndef SCENE_BATTLEFIELD_H
#define SCENE_BATTLEFIELD_H

#include"Layer_BattleField.h"
#include"Menu_BattleField.h"
#include"Sprite_CampaignInfo.h"

#include"GameScene.h"
#include"GameButton.h"
#include"CountDown.h"

#include"Campaign.h"

//地图编辑菜单
#define BATTLEFIELD_EDIT_MAP_MENU(MACRO) \
MACRO(Intel)/*地图信息*/\
MACRO(CorpSelect)/*选择兵种*/\
MACRO(TerrainSelect)/*选择地形*/\
MACRO(TroopSelect)/*选择势力*/\
MACRO(Delete)/*删除单位*/\
MACRO(Fill)/*填充*/\
MACRO(LoadFile)/*载入文件*/\
MACRO(SaveFile)/*保存文件*/\
MACRO(ExitMap)/*退出编辑*/\

#define BATTLEFIELD_CAMPAIGN_MENU(MACRO) \
MACRO(UnitsList)/*自军部队表*/\
MACRO(Mission)/*战役目标*/\
MACRO(BattleSituation)/*战况*/\
MACRO(Statistics)/*统计情况*/\
MACRO(Commander)/*指挥官*/\
MACRO(Rule)/*规则*/\
MACRO(Yield)/*投降*/\
MACRO(Delete)/*删除单位*/\
MACRO(Power)/*发动能力*/\
MACRO(ChangeCO)/*结束回合,并更换CO*/\
MACRO(EndTurn)/*结束回合*/\
MACRO(SaveFile)/*存档*/\
MACRO(LoadFile)/*读挡*/\
MACRO(ExitMap)/*退出地图*/\

//战场的场景,负责显示战场的信息
class Scene_BattleField:public GameScene{
	CountDown_FadeTo cdFadeTo;
	CountDown cdDelay;
public:
	//构造/析构函数
	Scene_BattleField();
	~Scene_BattleField();

	#undef ENUM
	#define ENUM(name) MapEdit_##name,
	enum EnumMapEditCommand{//枚举地图编辑命令
		BATTLEFIELD_EDIT_MAP_MENU(ENUM)
		AmountOfEnumMapEditCommand
	};
	#undef ENUM
	#define ENUM(name) Campaign_##name,
	enum EnumCampaignCommand{//枚举战役命令
		BATTLEFIELD_CAMPAIGN_MENU(ENUM)
		AmountOfEnumCampaignCommand
	};

	//地图数据
	BattleField *battleField;//这是战场,主要存地形和单位
	Campaign *campaign;//比赛
	Campaign::CoordType cursorPoint;//光标所处的坐标

	//控件
	Layer_BattleField layerBattleField;//战场图层
	Sprite_TerrainInfo spriteTerrainInfo;//地形信息框
	Sprite_UnitInfo spriteUnitInfo;//单位信息框
	GameButton_String buttonEdit;//编辑按钮,用来打开编辑菜单

	//编辑模式菜单
	Menu_MapEdit *menuMapEdit;//地图编辑菜单
	Menu_CorpSelect *menuCorpSelect;//兵种选择菜单
	Menu_TerrainSelect *menuTerrainSelect;//地形选择菜单
	Menu_TroopSelect *menuTroopSelect;//势力选择菜单

	//对战模式菜单
	Menu_Campaign *menuCampaign;//战役模式的菜单
	Menu_ProduceSelect *menuProduceSelect;//生产菜单,用于生产部队
	Menu_UnitSelect *menuUnitSelect;//单位表,用于查看场上单位
	Menu_CorpCommand *menuCorpCommand;//兵种命令菜单,用于显示命令

	//对战模式精灵组
	Sprite_CurrentDay *spriteCurrentDay;//显示当前天数和当前行动部队
	Sprite_TroopFundsCO *spriteTroopFundsCO;//显示资金和CO

	//模式
	void gotoEditMode();//进入编辑模式,对编辑菜单进行初始化
	void gotoBattleMode();//进入战斗模式

	void beginTurn();

	//输入事件
	virtual void reset();
	void setCursor(const Campaign::CoordType p);//设置光标位置

	//菜单
	void showMenu(GameMenu &menu,decltype(GameMenu::onConfirm) onConfirm,decltype(GameMenu::onCancel) onCancel=nullptr);
	void updateMenu();//获取菜单情况
};
#endif