#ifndef SCENE_BATTLEFIELD_H
#define SCENE_BATTLEFIELD_H

#include"GameScene.h"
#include"Layer_BattleField.h"
#include"Sprite_TerrainInfo.h"
#include"Sprite_UnitInfo.h"
#include"Menu_BattleField.h"

#include"GameButton.h"
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

//战场的场景,负责显示战场的信息
class Scene_BattleField:public GameScene{
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
	Menu_CorpSelect menuCorpSelect;//兵种选择菜单
	Menu_TerrainSelect menuTerrainSelect;//地形选择菜单
	Menu_TroopSelect menuTroopSelect;//势力选择菜单
	Menu_MapEdit menuMapEdit;//地图编辑菜单
	//对战模式菜单
	Menu_UnitSelect menuUnitSelect;
	Menu_CorpCommand menuCorpCommand;//兵种命令菜单,用于显示命令

	//编辑模式
	void gotoEditMode();//进入编辑模式,对编辑菜单进行初始化
	void gotoBattleMode();//进入战斗模式

	//输入事件
	virtual void reset();
	void setCursor(const Campaign::CoordType p);//设置光标位置

	//菜单
	void showMenu(GameMenu &menu,decltype(GameMenu::onConfirm) onConfirm,decltype(GameMenu::onCancel) onCancel=nullptr);
	void updateMenu();//获取菜单情况
};
#endif