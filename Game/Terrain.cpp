#include"Terrain.h"
#include"LuaState.h"

Terrain::Terrain(int type, int status){
	this->terrainType=type;
	this->status=status;
}

TerrainCode::TerrainCode():defendLV(0),capturable(false),hasIncome(false),buildable(false),hidable(false),has4direction(false){}

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
				READ_BOOL(capturable)
				READ_BOOL(hasIncome)
				READ_BOOL(buildable)
				READ_BOOL(hidable)
				READ_BOOL(has4direction)
				READ_STR(tileType)
				return true;
			});
		});
	}
	return ret;
}
bool TerrainCodesList::canAdjustTile(int terrainType)const{
	auto code=this->data(terrainType);
	return code ? code->has4direction : false;
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