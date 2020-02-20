#ifndef MENU_CORPCOMMAND_H
#define MENU_CORPCOMMAND_H

#include"GameMenu.h"
#include"Campaign.h"

//兵种命令菜单
class Menu_CorpCommand:public GameMenu{
public:
	Menu_CorpCommand();

	decltype(Campaign::corpMenu) *corpMenu;//数据源
	void updateData();
};
#endif