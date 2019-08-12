#ifndef TABLE_TROOPSDATA_H
#define TABLE_TROOPSDATA_H

#include"GameTable.h"
#include"Troop.h"

/*兵种数据表*/
class Table_TroopsData:public GameTable{
public:
	Table_TroopsData();
	~Table_TroopsData();

	//virtual uint columnWidth(uint col)const;
	//virtual void render()const;
	//数据源
};

#endif
