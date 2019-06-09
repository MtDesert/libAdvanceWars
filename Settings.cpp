#include"Settings.h"
#include"LuaState.h"
#include<string.h>

extern string errorString;

Settings::Settings(){}
Settings::~Settings(){}

#define SETTING_STRING(name)\
LUASTATE_ASSERT(luaState.getGlobalString(#name,name),"No "#name"!");

#define SETTING_FILENAME_IMAGES_PATH(name)\
SETTING_STRING(Filename##name)\
SETTING_STRING(ImagesPath##name)

bool Settings::saveFile(const string &filename)const{return false;}
bool Settings::loadFile(const string &filename){
	//尝试打开配置文件,出错了就直接返回错误信息
	LuaState luaState;
	LUASTATE_ASSERT(luaState.doFile(filename),luaState.errorString);
	SETTING_STRING(language);
	//数据文件&纹理路径
	SETTING_FILENAME_IMAGES_PATH(Corps)
	SETTING_FILENAME_IMAGES_PATH(Troops)
	SETTING_FILENAME_IMAGES_PATH(Commanders)
	SETTING_FILENAME_IMAGES_PATH(Terrains)
	//其他数据路径
	LUASTATE_ASSERT(luaState.getGlobalString("senarioScriptsPath",senarioScriptsPath),"No senario scripts path!");
	LUASTATE_ASSERT(luaState.getGlobalString("mapsPath",mapsPath),"No maps path!");
	luaState.clearStack();
	return true;
}