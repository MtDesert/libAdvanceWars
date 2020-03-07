#ifndef MENU_TERRAINSELECT_H
#define MENU_TERRAINSELECT_H

#include"GameMenu.h"
#include"Terrain.h"

//同时声明Name的菜单项class和菜单class,otherCodeOfItem和otherCodeOfMenu为其它成员的声明
#define CLASS_MENU_ITEM_DECLARE(Name,otherCodeOfItem,otherCodeOfMenu) \
class MenuItem_##Name:public GameMenuItem_IconName{\
public:\
	MenuItem_##Name();\
	void updateData(SizeType pos);\
	otherCodeOfItem\
};\
class Menu_##Name:public GameMenuTemplate<MenuItem_##Name>{\
public:\
	Menu_##Name();\
	virtual SizeType itemAmount()const;\
	otherCodeOfMenu\
};

//部队选择,主要用于设计地图
CLASS_MENU_ITEM_DECLARE(TroopSelect,,)
//兵种选择,可用于设计地图和生产
CLASS_MENU_ITEM_DECLARE(CorpSelect,
	GameString stringPrice;,//显示价格
	SizeType troopID;
)
//地形选择,主要用于设计地图
CLASS_MENU_ITEM_DECLARE(TerrainSelect,
	,
	SizeType troopID;
)
//CO选择
CLASS_MENU_ITEM_DECLARE(CommanderSelect,,)

//地图编辑菜单,用于操作
CLASS_MENU_ITEM_DECLARE(MapEdit,
	,
	Menu_CorpSelect *menuCorpSelect;
	Menu_TerrainSelect *menuTerrainSelect;
	Menu_TroopSelect *menuTroopSelect;
)

//兵种命令
CLASS_MENU_ITEM_DECLARE(CorpCommand,,)

#endif