#ifndef TABLE_UNITSDATA_H
#define TABLE_UNITSDATA_H

#include"GameTable.h"
#include"Unit.h"

/*单位数据表*/
class Table_UnitsData:public GameTable{
public:
	Table_UnitsData();
	~Table_UnitsData();

	virtual uint columnAmount()const;
	virtual uint columnWidth(uint col)const;
	virtual void render()const;
	//数据源
	List<Unit> *source;
	TextureCache *textures;
protected:
	virtual DataBlock headerBlock(uint col)const;
};

#endif
