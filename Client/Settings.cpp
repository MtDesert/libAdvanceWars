#include"Settings.h"

Settings::Settings(){}
Settings::~Settings(){}

#define READ_STR(name) state.getGlobalString(#name,name);

bool Settings::saveFile(const string &filename)const{return false;}
void Settings::readCustom(LuaState &state){
	READ_STR(mapsPath)
	READ_STR(scenarioScriptsPath)
}