#include"Terrain.h"
#include"LuaState.h"

Terrain::Terrain(int type, int status){
	this->terrainType=type;
	this->status=status;
}

TerrainCode::TerrainCode():defendLV(0),capturable(false),hasIncome(false),buildable(false),hidable(false),has4direction(false){}

string TerrainCodeList::loadFile_lua(const string &filename){
	lua_State *state=luaL_newstate();
	//加载文件
	//搜索地形表
	lua_getglobal(state,"Terrains");
	if(lua_gettop(state)!=1)return("No table \'Terrains\'");
	if(!lua_istable(state,1))return("\'Terrains\' not table");
	//遍历地形表
	lua_pushnil(state);//遍历前要push nil
	while(lua_next(state,-2)){//-2是key的位置
		if(lua_istable(state,-1)){//-1是value的位置
			TerrainCode terrain;//读取用的缓冲
			lua_pushnil(state);
			while(lua_next(state,-2)){
				string str=lua_tostring(state,-2);
				if(str=="name"){//兵种名称
					terrain.name=lua_tostring(state,-1);
				}else if(str=="defendLV"){
					terrain.defendLV=lua_tointeger(state,-1);
				}else if(str=="capturable"){
					terrain.capturable=lua_toboolean(state,-1);
				}else if(str=="hasIncome"){
					terrain.hasIncome=lua_toboolean(state,-1);
				}else if(str=="buildable"){
					terrain.buildable=lua_toboolean(state,-1);
				}else if(str=="hidable"){
					terrain.hidable=lua_toboolean(state,-1);
				}else if(str=="has4direction"){
					terrain.has4direction=lua_toboolean(state,-1);
				}else if(str=="tileType"){
					terrain.tileType=lua_tostring(state,-1);
				}
				lua_pop(state,1);
			}
			push_back(terrain);
		}
		lua_pop(state,1);
	}
	//finish
	return "";
}
bool TerrainCodeList::canAdjustTile(int terrainType)const{
	auto code=List<TerrainCode>::data(terrainType);
	if(code){
		return code->has4direction;
	}
	return false;
}
bool TerrainCodeList::canAdjustTile(int terrainTypeA,int terrainTypeB)const{
	auto codeA=List<TerrainCode>::data(terrainTypeA);
	if(!codeA)return false;
	auto codeB=List<TerrainCode>::data(terrainTypeB);
	if(codeB){
		if(codeA->tileType=="Way" && codeB->capturable){return true;}
		if(codeA->tileType=="Water" && codeB->name=="Bridge"){return true;}//special...
		if(codeA->name=="Bridge" && codeB->tileType!="Water"){return true;}//special...
		return codeA->tileType==codeB->tileType;
	}
	return false;
}

/*bool TerrainCodeList::canAdjustTile(_List_const_iterator<TerrainCode> itrA,
									_List_const_iterator<TerrainCode> itrB)const{
	if(itrA==end()||itrB==end())return false;
	//found terrain
	if(itrA->tileType=="Way" && itrB->style==2){return true;}//road/bridge & capturable
	if(itrA->tileType == itrB->tileType){return true;}//same type
	if(itrA->tileType=="Water" && itrB->name=="Bridge"){return true;}//special...
	if(itrA->name=="Bridge" && itrB->tileType!="Water"){return true;}//special...
	return false;
}*/
