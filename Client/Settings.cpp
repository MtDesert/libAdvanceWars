#include"Settings.h"

Settings::Settings(){}
Settings::~Settings(){}

#define READ_STR(name) state.getGlobalString(#name,name);

bool Settings::saveFile(const string &filename)const{return false;}
void Settings::readCustom(LuaState &state){
	//数据文件
	READ_STR(dataTerrainCodes)
	READ_STR(dataCorps)
	READ_STR(dataCommanders)
	READ_STR(dataTroops)
	READ_STR(dataWeathers)
	//规则文件
	READ_STR(ruleMove)
	READ_STR(ruleDamage)
	READ_STR(ruleLoadUnit)
	READ_STR(ruleBuild)
	READ_STR(ruleCommanders)
	//图像目录
	READ_STR(imagesPathTerrainCodes)
	READ_STR(imagesPathCorps)
	READ_STR(imagesPathCorpsImages)
	READ_STR(imagesPathTroops)
	READ_STR(imagesPathCommanders)
	READ_STR(imagesPathIcons)
	READ_STR(imagesPathNumbers)
	//文件选择框路径
	READ_STR(mapsPath)
	READ_STR(scenarioScriptsPath)
	READ_STR(savesPath)
}