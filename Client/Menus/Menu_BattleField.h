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
	virtual SizeType rowAmount()const;\
	otherCodeOfMenu\
};

CLASS_MENU_ITEM_DECLARE(CorpSelect,
	GameString stringPrice;,
	SizeType troopID;
)
CLASS_MENU_ITEM_DECLARE(TerrainSelect,,SizeType troopID;)
CLASS_MENU_ITEM_DECLARE(TroopSelect,,)
CLASS_MENU_ITEM_DECLARE(CorpCommand,,)

#endif