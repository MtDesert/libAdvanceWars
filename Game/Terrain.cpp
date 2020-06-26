#include"Terrain.h"
#include"LuaState.h"
#include"Number.h"

Terrain::Terrain(int type,int status):unitIndex(TERRAIN_NO_UNIT){
	this->terrainType=type;
	this->status=status;
	isVisible=true;
}

TerrainCode::TerrainCode():defendLV(0),attackPercent(0),capturable(false),hasIncome(false),hidable(false),has4direction(false){}

#define WRITE_STR(name) ret+=#name"=\""+name+"\",";
#define WRITE_INT(name) ret+=#name"="+Number::toString(name)+",";
#define WRITE_BOOL(name) if(name)ret+=#name"=true,";

string TerrainCode::toLuaString()const{
	string ret("{");
	WRITE_STR(name)
	WRITE_STR(translate)
	WRITE_INT(defendLV)
	WRITE_INT(attackPercent)
	WRITE_STR(repairType)
	WRITE_STR(produceType)
	WRITE_STR(terrainAfterLanuch)
	//
	WRITE_BOOL(capturable)
	WRITE_BOOL(hasIncome)
	WRITE_BOOL(hidable)
	WRITE_BOOL(has4direction)
	WRITE_STR(tileType)
	ret+="}";
	return ret;
}

#define READ_BOOL(name) code->name=state.getTableBoolean(#name);
#define READ_INT(name) code->name=state.getTableInteger(#name);
#define READ_STR(name) state.getTableString(#name,code->name);

bool TerrainCodesList::loadFile_lua(const string &filename,WhenErrorString callback){
	bool ret=true;
	LuaState state;
	state.whenError=callback;
	//加载文件,读取Terrains表
	if(state.doFile(filename)){
		ret=state.getGlobalTable("Terrains",[this,&state](){
			setSize(state.getTableLength(),true);//申请空间
			return state.getTableForEach([this,&state](int index){
				auto code=this->data(index);
				if(!code)return false;
				//读取
				READ_STR(name)
				READ_STR(translate)
				READ_INT(defendLV)
				READ_INT(attackPercent)
				READ_STR(repairType)
				READ_STR(produceType)
				READ_STR(terrainAfterLanuch)
				READ_BOOL(capturable)
				READ_BOOL(hasIncome)
				READ_BOOL(hidable)
				READ_BOOL(has4direction)
				READ_STR(tileType)
				return true;
			});
		});
	}
	return ret;
}
bool TerrainCodesList::canAdjustTile(int terrainTypeA,int terrainTypeB)const{
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