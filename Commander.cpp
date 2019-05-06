#include"Commander.h"
#include"LuaState.h"

#define COMMANDER_ATTR(luaname,cppname,type)\
if(str==#luaname){co.cppname=lua_to##type(state,-1);}else

string CommandersList::loadFile_lua(const string &filename){
	lua_State *state=luaL_newstate();
	//加载文件
	//搜索CO表
	lua_getglobal(state,"Commanders");
	if(lua_gettop(state)!=1)return("No table \'Commanders\'");
	if(!lua_istable(state,1))return("\'Commanders\' not table");
	//遍历CO表
	lua_pushnil(state);//遍历前要push nil
	while(lua_next(state,-2)){//-2是key的位置
		if(lua_istable(state,-1)){//-1是value的位置
			Commander co;//读取用的缓冲
			lua_pushnil(state);
			while(lua_next(state,-2)){
				string str=lua_tostring(state,-2);
				COMMANDER_ATTR(name,coName,string)
				COMMANDER_ATTR(note,note,string)
				COMMANDER_ATTR(quote,quote,string)
				COMMANDER_ATTR(hit,coHit,string)
				COMMANDER_ATTR(miss,coMiss,string)
				COMMANDER_ATTR(d2d,d2d,string)
				COMMANDER_ATTR(copName,power.name,string)
				COMMANDER_ATTR(copStar,power.starCount,integer)
				COMMANDER_ATTR(copNote,power.note,string)
				COMMANDER_ATTR(scopName,superPower.name,string)
				COMMANDER_ATTR(scopStar,superPower.starCount,integer)
				COMMANDER_ATTR(scopNote,superPower.note,string)
				{}
				lua_pop(state,1);
			}
			push_back(co);
		}
		lua_pop(state,1);
	}
	//finish
	return "";
}
