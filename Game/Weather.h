#ifndef WEATHER_H
#define WEATHER_H

#include"DataList.h"

/*部队数据,用来描述*/
struct Weather{
	string name;//部队名称
};

class WeathersList:public DataList<Weather>{
public:
	int loadFile_lua(const string &filename);//加载lua格式的数据文件,成功的时候返回LUA_OK
};

#endif
