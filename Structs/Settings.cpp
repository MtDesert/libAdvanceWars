#include"Settings.h"
#include"LuaState.h"
#include<string.h>

Settings::Settings(){}
Settings::~Settings(){}

#define RETURN(name)\
{lua_close(state);\
return name;}

#define OPEN_LUA_FILE \
lua_State *state=luaL_newstate();

static void toStringList(lua_State *state,const string &tableName,StringList &stringList){
	stringList.clear();//清理旧数据
	lua_getglobal(state,tableName.data());
	if(lua_istable(state,1)){
		lua_pushnil(state);
		while(lua_next(state,-2)){
			if(lua_isstring(state,-1)){
				stringList.push_back(lua_tostring(state,-1));
			}
			lua_pop(state,1);
		}
	}
}

#define SETTINGS_ATTR(luaname,cppname,type)\
if(str==#luaname){cppname=lua_to##type(state,-1);}else

bool Settings::saveFile(const string &filename)const{return false;}
string Settings::loadFile(const string &filename){
	//尝试打开配置文件,出错了就直接返回错误信息
	OPEN_LUA_FILE
	//读取数据文件名
	toStringList(state,"CorpFilenames",filenamesCorps);
	toStringList(state,"TroopFilenames",filenamesTroops);
	toStringList(state,"CommanderFilenames",filenamesCommanders);
	toStringList(state,"TerrainFilenames",filenamesTerrainCode);
	//读取资源路径名
	lua_getglobal(state,"imagesPath");
	if(lua_istable(state,-1)){
		lua_pushnil(state);
		while(lua_next(state,-2)){
			if(lua_isstring(state,-1)){
				string str=lua_tostring(state,-2);
				SETTINGS_ATTR(Corps,imagePathCorps,string);
				SETTINGS_ATTR(Troops,imagePathTroops,string);
				SETTINGS_ATTR(CommandersHeads,imagePathCommanders,string);
				SETTINGS_ATTR(CommandersBodies,imagePathCommandersBodies,string);
			}
			lua_pop(state,1);
		}
	}
	RETURN("");
}

static lua_State *translationState=nullptr;
string Settings::loadTranslationFile(const string &filename){
	LuaState luaState;
	luaState.doFile(filename);
	OPEN_LUA_FILE
	translationState=state;
	return"";
}
void Settings::closeTranslationFile(){
	if(translationState){
		lua_close(translationState);
		translationState=nullptr;
	}
}

const char* Settings::translate(const string &english)const{
	const char* ret="";
	lua_getglobal(translationState,"translate");
	lua_pushstring(translationState,english.data());
	if(lua_pcall(translationState,1,1,0)==LUA_OK){
		if(!lua_isnil(translationState,-1)){
			ret=lua_tostring(translationState,-1);
		}
	}else{
		printf("%s\n",lua_tostring(translationState,-1));
	}
	return ret;
}
