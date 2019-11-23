#include"Settings.h"
#include"LuaState.h"
#include<string.h>

Settings::Settings():serverPort(0){}
Settings::~Settings(){}

#define SETTING_STRING(name)\
luaState.getGlobalString(#name,name);

#define SETTING_FILENAME_IMAGES_PATH(name)\
SETTING_STRING(Filename##name)\
SETTING_STRING(ImagesPath##name)

bool Settings::saveFile(const string &filename)const{return false;}
bool Settings::loadFile(const string &filename,void (*whenError)(const string &errStr)){
	//尝试打开配置文件,出错了就直接返回错误信息
	LuaState luaState;
	luaState.whenError=whenError;
	luaState.doFile(filename);
	SETTING_STRING(language);
	//数据文件&纹理路径
	SETTING_FILENAME_IMAGES_PATH(Corps)
	SETTING_FILENAME_IMAGES_PATH(Troops)
	SETTING_FILENAME_IMAGES_PATH(Commanders)
	SETTING_FILENAME_IMAGES_PATH(Terrains)
	//其他数据路径
	SETTING_STRING(senarioScriptsPath)
	SETTING_STRING(mapsPath)
	//网络部分
	SETTING_STRING(serverAddress);
	luaState.getGlobalInteger("serverPort",serverPort);
	//结束
	luaState.clearStack();
	return true;
}