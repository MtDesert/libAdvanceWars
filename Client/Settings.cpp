#include"Settings.h"

Settings::Settings(){}
Settings::~Settings(){}

#define READ_STR(name) state.getGlobalString(#name,name);

bool Settings::saveFile(const string &filename)const{return false;}
void Settings::readCustom(LuaState &state){
	//配置文件
	READ_STR(dataTerrainCodes)
	READ_STR(dataCorps)
	READ_STR(dataCommanders)
	READ_STR(dataTroops)
	//图像目录
	READ_STR(imagesPathTerrainCodes)
	READ_STR(imagesPathCorps)
	READ_STR(imagesPathTroops)
	READ_STR(imagesPathCommanders)
	//文件选择框路径
	READ_STR(mapsPath)
	READ_STR(scenarioScriptsPath)
}