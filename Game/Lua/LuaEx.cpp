#include"LuaEx.h"

#define RETURN_IF_ERROR \
if(result!=LUA_OK){\
	string ret=lua_tostring(state,-1);\
	lua_close(state);\
	return ret;\
}

string loadfile_pcall(lua_State *state,const string &filename){
	//加载文件
	int result=luaL_loadfile(state,filename.data());
	RETURN_IF_ERROR
	//保护调用
	result=lua_pcall(state,0,LUA_MULTRET,0);
	RETURN_IF_ERROR
	return "";
}
