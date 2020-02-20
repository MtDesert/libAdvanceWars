#include"Troop.h"
#include"LuaState.h"

Troop::Troop(){}

#define READ_BOOL(name) troop->name=state.getTableBoolean(#name);
#define READ_INT(name) troop->name=state.getTableInteger(#name);
#define READ_STR(name) state.getTableString(#name,troop->name);

bool TroopsList::loadFile_lua(const string &filename,WhenErrorString whenError){
	LuaState state;
	state.whenError=whenError;
	if(state.doFile(filename)){
		return state.getGlobalTable("Troops",[&](){
			setSize(state.getTableLength(),true);
			return state.getTableForEach([&](int index){
				auto troop=this->data(index);
				if(!troop)return false;
				//读取数据
				READ_STR(name)
				READ_STR(translate)
				return state.getTableTable("colors",[&](){
					return state.getTableForEach([&](int idx){
						if(idx<4){
							troop->colors[idx]=state.getTopInteger();
						}
						return true;
					});
				});
			});
		});
	}
	return false;
}