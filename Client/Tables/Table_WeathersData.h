#ifndef TABLE_WEATHERSDATA_H
#define TABLE_WEATHERSDATA_H

#include"GameTable.h"
#include"Corp.h"

/*兵种数据表*/
class Table_WeathersData:public GameTable{
public:
	Table_WeathersData();
	~Table_WeathersData();

	//virtual uint columnWidth(uint col)const;
	//virtual void render()const;
	//数据源
	CorpsList *source;
};

#endif
