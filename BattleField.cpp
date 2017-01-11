#include "BattleField.h"

#include<stdlib.h>
#include<fstream>
#include<iostream>
#include"lua.hpp"
#include"tinyxml2.h"
using namespace tinyxml2;

BattleField::BattleField(){}

bool BattleField::getTerrain(uint x,uint y,Terrain &terrain)const{return getValue(x,y,terrain);}
bool BattleField::getTerrain(const Point &p,Terrain &terrain)const{return getTerrain(p.x,p.y,terrain);}
bool BattleField::setTerrain(uint x,uint y,const Terrain &terrain){return setValue(x,y,terrain);}
bool BattleField::setTerrain(const Point &p,const Terrain &terrain){return setTerrain(p.x,p.y,terrain);}
bool BattleField::fillTerrain(const Terrain &terrain)
{
	for(unsigned x=0;x<length;++length)
	{
		for(unsigned y=0;y<width;++y)
		{
			setValue(x,y,terrain);
		}
	}
	return true;
}

void BattleField::autoAdjustTerrainsTiles(){
	for(uint x=0;x<length;++x){
		for(uint y=0;y<width;++y){
			autoAdjustTerrainTile(x,y);
		}
	}
}
void BattleField::autoAdjustTerrainTile(uint x, uint y){
	Terrain terrain;
	if(!getTerrain(x,y,terrain))return;
	auto itrA=terrainCodes.iterate(terrain.terrainType-1);
	if(itrA==terrainCodes.end())return;
	if(itrA->style!=1)return;
	//find terrain around
	terrain.status=0;
	uchar status=1;
	Terrain beside;
	List<Point> pList;
	pList.push_back(Point(x,y-1));
	pList.push_back(Point(x,y+1));
	pList.push_back(Point(x-1,y));
	pList.push_back(Point(x+1,y));
	for(auto itrP=pList.begin();itrP!=pList.end();++itrP,status*=2){
		if(getTerrain(*itrP,beside)){
			auto itrB=terrainCodes.iterate(beside.terrainType-1);
			if(terrainCodes.canAdjustTile(itrA,itrB)){
				terrain.status+=status;
			}
		}
	}
	setTerrain(x,y,terrain);
}

bool BattleField::saveFile(const String &filename)const
{
	fstream mapFile;
	mapFile.open(filename.data(),ios_base::binary|ios_base::out);
	if(!mapFile.is_open())return false;
	//read
	mapFile.write((char*)&length,2);
	mapFile.write((char*)&width,2);
	for(unsigned x=0;x<length;++x)
	{
		for(unsigned y=0;y<width;++y)
		{
			mapFile.write((char*)&data[x][y],2);
		}
	}
	saveUnitList(mapFile,chessPieces);
	//over
	mapFile.close();
	return true;
}
bool BattleField::loadFile(const String &filename)
{
	fstream mapFile;
	mapFile.open((mapPath+filename).data(),ios_base::binary|ios_base::in);
	if(!mapFile.is_open())return false;
	//read
	ushort w,h;
	mapFile.read((char*)&w,2);
	mapFile.read((char*)&h,2);
	initWith(w,h);
	for(unsigned x=0;x<length;++x)
	{
		for(unsigned y=0;y<width;++y)
		{
			mapFile.read((char*)&data[x][y],2);
		}
	}
	chessPieces.clear();
	loadUnitList(mapFile,chessPieces);
	//over
	mapFile.close();
	return true;
}
int BattleField::loadMap_XML(const String &filename){
	XMLDocument document;
	XMLError error=document.LoadFile(filename.data());
	if(error)return error;
	//find terrains
	auto root=document.RootElement();
	auto terrains=root->FirstChildElement("Terrains");
	if(!terrains)return -1;
	//get size
	uint w=terrains->IntAttribute("width");
	uint h=terrains->IntAttribute("height");
	initWith(w,h);
	//scan terrain
	for(auto trn=terrains->FirstChildElement("Terrain");trn;trn=trn->NextSiblingElement("Terrain")){
		int x=trn->IntAttribute("x");
		int y=trn->IntAttribute("y");
		const char *name=trn->Attribute("name");
		if(!name)name="";
		//scan terrain id
		int terrainID=1;
		auto itrTerrainCode=terrainCodes.begin();
		for(;itrTerrainCode!=terrainCodes.end();++itrTerrainCode,++terrainID){
			if(itrTerrainCode->name==name)break;
		}
		//not found...pass
		if(itrTerrainCode==terrainCodes.end())continue;
		//check terrain style
		switch(itrTerrainCode->style){
			case 0:case 1:{
				Terrain terrain(terrainID);
				setTerrain(x,y,terrain);
			}break;
			case 2:{//capturable terrain
				const char *troop=trn->Attribute("troop");
				if(!troop)troop="Neutral";
				int troopID=1;
				auto itrTroopCode=troopCodes.begin();
				for(;itrTroopCode!=troopCodes.end();++itrTroopCode,++troopID){
					if(itrTroopCode->name==troop)break;
				}
				//not found...pass
				if(itrTroopCode==troopCodes.end())continue;
				//
				Terrain terrain(terrainID,troopID);
				setTerrain(x,y,terrain);
			}break;
		}
		cout<<x<<","<<y<<"--"<<name<<endl;
		cout.flush();
	}
	cout<<"map over"<<endl;cout.flush();
	return 0;
}
bool BattleField::loadMap_LUA(const String &filename){return false;}
bool BattleField::loadMap_CSV(const String &filename){return false;}

bool BattleField::saveMap_XML(const String &filename)const{return false;}
bool BattleField::saveMap_LUA(const String &filename)const{return false;}
bool BattleField::saveMap_CSV(const String &filename)const{return false;}

bool BattleField::loadCorps_XML(const String &filename){return false;}
int BattleField::loadCorps_LUA(const String &filename){
	lua_State *state=luaL_newstate();
	//load file
	int loadResult=luaL_loadfile(state,filename.data());
	if(loadResult!=LUA_OK){return loadResult;}
	//protect call
	int pcallResult=lua_pcall(state,0,LUA_MULTRET,0);
	if(pcallResult!=LUA_OK){return pcallResult;}
	//scan Terrains
	lua_getglobal(state,"Corps");
	if(lua_gettop(state)!=1)return -1;
	if(!lua_istable(state,1))return -2;
	//scan each terrain info
	lua_pushnil(state);
	while(lua_next(state,-2)){
		if(lua_istable(state,-1)){
			//it is a terrain info,scan each attrib
			lua_pushnil(state);
			while(lua_next(state,-2)){
				if(lua_tostring(state,-2)==string("name")){
					corpCodes.push_back(lua_tostring(state,-1));
				}
				lua_pop(state,1);
			}
		}
		lua_pop(state,1);
	}
	//finish
	lua_close(state);
	return LUA_OK;
}

bool BattleField::loadTerrains_XML(const String &filename){return false;}
int BattleField::loadTerrains_LUA(const String &filename){
	lua_State *state=luaL_newstate();
	//load file
	int loadResult=luaL_loadfile(state,filename.data());
	if(loadResult!=LUA_OK){return loadResult;}
	//protect call
	int pcallResult=lua_pcall(state,0,LUA_MULTRET,0);
	if(pcallResult!=LUA_OK){return pcallResult;}
	//scan Terrains
	lua_getglobal(state,"Terrains");
	if(lua_gettop(state)!=1)return -1;
	if(!lua_istable(state,1))return -2;
	//scan each terrain info
	lua_pushnil(state);
	while(lua_next(state,-2)){
		if(lua_istable(state,-1)){
			//it is a terrain info,scan each attrib
			TerrainCode code;
			code.style=0;
			lua_pushnil(state);
			while(lua_next(state,-2)){
				string attribName=lua_tostring(state,-2);
				if(attribName==string("name")){
					code.name=lua_tostring(state,-1);
				}else if(attribName==string("has4direction") && lua_toboolean(state,-1)){
					code.style=1;
				}else if(attribName==string("capturable") && lua_toboolean(state,-1)){
					code.style=2;
				}else if(attribName==string("tileType") && lua_isstring(state,-1)){
					code.tileType=lua_tostring(state,-1);
				}
				lua_pop(state,1);
			}
			terrainCodes.push_back(code);
		}
		lua_pop(state,1);
	}
	//finish
	lua_close(state);
	return LUA_OK;
}
bool BattleField::loadTroops_XML(const String &filename){return false;}
int BattleField::loadTroops_LUA(const String &filename){
	lua_State *state=luaL_newstate();
	//load file
	int loadResult=luaL_loadfile(state,filename.data());
	if(loadResult!=LUA_OK){return loadResult;}
	//protect call
	int pcallResult=lua_pcall(state,0,LUA_MULTRET,0);
	if(pcallResult!=LUA_OK){return pcallResult;}
	//scan Terrains
	lua_getglobal(state,"Troops");
	if(lua_gettop(state)!=1)return -1;
	if(!lua_istable(state,1))return -2;
	//scan each troop info
	lua_pushnil(state);
	while(lua_next(state,-2)){
		if(lua_istable(state,-1)){
			//it is a troop info,scan each
			TroopCode code;
			lua_pushnil(state);
			while(lua_next(state,-2)){
				string attribName=lua_tostring(state,-2);
				if(attribName==string("name")){
					code.name=lua_tostring(state,-1);
				}else if(attribName==string("colors")){
					//scan colors
					lua_pushnil(state);
					while(lua_next(state,-2)){
						int index=lua_tonumber(state,-2);
						if(index>=1&&index<=4){
							code.colors[index-1]=lua_tonumber(state,-1);
						}
						lua_pop(state,1);
					}
				}
				lua_pop(state,1);
			}
			troopCodes.push_back(code);
		}
		lua_pop(state,1);
	}
	//finish
	lua_close(state);
	return LUA_OK;
}

string BattleField::mapPath;

void BattleField::saveUnitList(fstream &stream,const List<Unit> &unitList)const
{
	unsigned unitListSize=unitList.size();
	stream.write((char*)&unitListSize,4);
	for(_List_const_iterator<Unit> itr=unitList.begin();itr!=unitList.end();++itr)
	{
		stream.write((char*)(&(*itr)),sizeof(Unit)-sizeof(List<Unit>));
		saveUnitList(stream,itr->loadedUnits);
	}
}
void BattleField::loadUnitList(fstream &stream,List<Unit> &unitList)
{
	unsigned unitListSize=0;
	stream.read((char*)&unitListSize,4);
	Unit unit;
	for(unsigned i=0;i<unitListSize;++i)
	{
		unitList.push_back(unit);
		Unit &refUnit(unitList.back());
		stream.read((char*)&refUnit,sizeof(Unit)-sizeof(List<Unit>));
		loadUnitList(stream,refUnit.loadedUnits);
	}
}
