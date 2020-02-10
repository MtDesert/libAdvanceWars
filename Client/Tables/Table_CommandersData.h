#ifndef TABLE_COMMANDERSDATA_H
#define TABLE_COMMANDERSDATA_H

#include"GameTable.h"
#include"Commander.h"

/*兵种数据表*/
class Table_CommandersData:public GameTable{
protected:
	//缓冲区域
	struct ItemBuffer{
		Texture icon;
		DataBlock name,quote;
	};
	ItemBuffer *itemBuffers;
public:
	Table_CommandersData();
	~Table_CommandersData();

	uint columnAmount()const;
	uint columnWidth(uint col)const;
};
#endif