#include"Settings.h"
#include"LuaState.h"
#include<string.h>

extern string errorString;

Settings::Settings(){}
Settings::~Settings(){}

bool Settings::saveFile(const string &filename)const{return false;}
bool Settings::loadFile(const string &filename){
	//尝试打开配置文件,出错了就直接返回错误信息
	LuaState luaState;
	LUASTATE_ASSERT(luaState.doFile(filename),luaState.errorString);
	LUASTATE_ASSERT(luaState.getGlobalString("language",language),"No language!");
	LUASTATE_ASSERT(luaState.readEnum("CorpFilenames",filenamesCorps),luaState.errorString);
	LUASTATE_ASSERT(luaState.readEnum("TroopFilenames",filenamesTroops),luaState.errorString);
	LUASTATE_ASSERT(luaState.readEnum("CommanderFilenames",filenamesCommanders),luaState.errorString);
	LUASTATE_ASSERT(luaState.readEnum("TerrainFilenames",filenamesTerrainCode),luaState.errorString);
	//读取资源路径名
	return true;
}