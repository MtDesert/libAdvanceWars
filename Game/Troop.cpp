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
	/*lua_State *state=luaL_newstate();
	//加载文件
	//搜索部队表
	lua_getglobal(state,"Troops");
	if(lua_gettop(state)!=1)return("No table \'Troops\'");
	if(!lua_istable(state,1))return("\'Troops\' not table");
	//遍历部队表
	lua_pushnil(state);//遍历前要push nil
	while(lua_next(state,-2)){//-2是key的位置
		if(lua_istable(state,-1)){//-1是value的位置
			Troop troop;//读取用的缓冲
			lua_pushnil(state);
			while(lua_next(state,-2)){
				string str=lua_tostring(state,-2);
				if(str=="name"){//兵种名称
					troop.name=lua_tostring(state,-1);
				}else if(str=="colors" && lua_istable(state,-1)){
					lua_pushnil(state);
					int i=0;
					while(lua_next(state,-2)){
						if(lua_isinteger(state,-2)){
							troop.colors[i]=lua_tointeger(state,-1);
							++i;
						}
						lua_pop(state,1);
					}
				}
				lua_pop(state,1);
			}
			push_back(troop);
		}
		lua_pop(state,1);
	}*/
}