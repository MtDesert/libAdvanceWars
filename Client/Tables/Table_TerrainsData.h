#ifndef TABLE_TERRAINSDATA_H
#define TABLE_TERRAINSDATA_H

#include"GameTable.h"
#include"Terrain.h"

/*兵种数据表*/
class Table_TerrainsData:public GameTable{
public:
	Table_TerrainsData();
	~Table_TerrainsData();

	//virtual uint columnWidth(uint col)const;
	//virtual void render()const;
	//数据源
};

#endif
