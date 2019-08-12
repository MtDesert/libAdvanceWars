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
	//渲染
	void renderItem(const List<Commander>::iterator &itr,uint row,uint column,const Rectangle2D<float> &itemRect)const;
public:
	Table_CommandersData();
	~Table_CommandersData();

	uint columnAmount()const;
	uint columnWidth(uint col)const;

	TextureCache *textures;//显示头像用的纹理
};

#endif
