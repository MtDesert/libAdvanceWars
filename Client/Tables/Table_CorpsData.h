#ifndef TABLE_CORPSDATA_H
#define TABLE_CORPSDATA_H

#include"GameTable.h"
#include"Corp.h"

/*兵种数据表*/
class Table_CorpsData:public GameTable{
public:
	Table_CorpsData();
	~Table_CorpsData();

	virtual uint columnAmount()const;
	virtual uint columnWidth(uint col)const;
	virtual void render()const;
	//数据源
	CorpsList *source;
	int troopID;//兵种势力ID,用于让图标有特定的颜色
protected:
	virtual DataBlock headerBlock(uint col)const;
};

#endif
