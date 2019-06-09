#include"Terrain.h"
#include"LuaState.h"

Terrain::Terrain(int type, int status){
	this->terrainType=type;
	this->status=status;
}

TerrainCode::TerrainCode():defendLV(0),capturable(false),hasIncome(false),buildable(false),hidable(false),has4direction(false){}

#define LUA_TO_TYPE(name,type)\
if(str==#name){\
	terrain.name=lua_to##type(state,-1);\
}else\

string TerrainsList::loadFile_lua(const string &filename){
	LUASTATE_OPENFILE(filename.data());
	//搜索地形表
	lua_getglobal(state,"Terrains");
	if(lua_gettop(state)!=1)return("No table \'Terrains\'");
	if(!lua_istable(state,1))return("\'Terrains\' not table");
	//遍历地形表
	LUASTATE_TABLE_FOREACH(state,
		if(lua_istable(state,-1)){//-1是value的位置
			TerrainCode terrain;//读取用的缓冲
			LUASTATE_TABLE_FOREACH(state,
				string str=lua_tostring(state,-2);
				LUA_TO_TYPE(name,string)
				LUA_TO_TYPE(defendLV,integer)
				LUA_TO_TYPE(capturable,boolean)
				LUA_TO_TYPE(hasIncome,boolean)
				LUA_TO_TYPE(buildable,boolean)
				LUA_TO_TYPE(hidable,boolean)
				LUA_TO_TYPE(has4direction,boolean)
				LUA_TO_TYPE(tileType,string)
				{}
			)
			push_back(terrain);
		}
	);
	//关闭文件
	lua_close(state);
	return "";
}
bool TerrainsList::canAdjustTile(int terrainType)const{
	auto code=List<TerrainCode>::data(terrainType);
	if(code){
		return code->has4direction;
	}
	return false;
}
bool TerrainsList::canAdjustTile(int terrainTypeA,int terrainTypeB)const{
	auto codeA=this->data(terrainTypeA);
	if(!codeA)return false;
	auto codeB=this->data(terrainTypeB);
	if(codeB){
		if(codeA->tileType=="Way" && codeB->capturable){return true;}
		if(codeA->tileType=="Water" && codeB->name=="Bridge"){return true;}//special...
		if(codeA->name=="Bridge" && codeB->tileType!="Water"){return true;}//special...
		return codeA->tileType==codeB->tileType;
	}
	return false;
}