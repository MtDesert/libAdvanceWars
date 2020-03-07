#ifndef WEATHER_H
#define WEATHER_H

#include"DataList.h"

/*天气数据表*/
struct Weather{
	Weather();

	string name;//名称
	string translate;//翻译
	bool isRainy:1;//是否雨天
	bool isSnowy:1;//是否雪天
	bool isSandy:1;//是否风沙天
};

class WeathersList:public DataList<Weather>{
public:
	bool loadFile_lua(const string &filename,WhenErrorString whenError);//加载lua格式的数据文件,成功的时候返回LUA_OK
};
#endif