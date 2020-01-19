#ifndef TROOP_H
#define TROOP_H

#include"DataList.h"

/*部队数据,用来描述部队的一些信息*/
struct Troop{
	Troop();

	string name,translate;//部队名称及其译文
	uint32 colors[4];//4种层次的颜色
};

class TroopsList:public DataList<Troop>{
public:
	bool loadFile_lua(const string &filename,WhenErrorString whenError);//加载lua格式的数据文件,返回错误信息(返回nullptr表示无错误)
};
#endif