#include"Troop.h"
#include"LuaState.h"

#define RETURN(name)\
{lua_close(state);\
return name;}

string TroopsList::loadFile_lua(const string &filename){
	lua_State *state=luaL_newstate();
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
	}
	//finish
	RETURN("")
}
