#include"Weather.h"
#include"LuaState.h"

#define READ_BOOL(name) weather->name=state.getTableBoolean(#name);
#define READ_INT(name) weather->name=state.getTableInteger(#name);
#define READ_STR(name) state.getTableString(#name,weather->name);

Weather::Weather():isRainy(false),isSnowy(false),isSandy(false){}
bool WeathersList::loadFile_lua(const string &filename,WhenErrorString whenError){
	LuaState state;
	state.whenError=whenError;
	if(state.doFile(filename)){
		return state.getGlobalTable("Weathers",[&](){
			setSize(state.getTableLength(),true);
			return state.getTableForEach([&](int index){
				auto weather=this->data(index);
				if(!weather)return false;
				//读取数据
				READ_STR(name)
				READ_STR(translate)
				READ_BOOL(isRainy)
				READ_BOOL(isSnowy)
				READ_BOOL(isRainy)
				return true;
			});
		});
	}
	return false;
	return 0;
}