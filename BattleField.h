#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include"Terrain.h"
#include"Unit.h"

#include"ChessBoard.h"

#include"Array.h"
#include"List.h"
#include"String.h"
#include"NS_Export.h"

class BattleField:public ChessBoard<Terrain,Unit>
{
public:
	explicit BattleField();
	//file
	bool saveFile(const String &filename)const;
	bool loadFile(const String &filename);

	//load map
	int loadMap_XML(const String &filename);
	bool loadMap_LUA(const String &filename);
	bool loadMap_CSV(const String &filename);
	//save map
	bool saveMap_XML(const String &filename)const;
	bool saveMap_LUA(const String &filename)const;
	bool saveMap_CSV(const String &filename)const;

	//load corps
	bool loadCorps_XML(const String &filename);
	int loadCorps_LUA(const String &filename);
	//load terrains
	bool loadTerrains_XML(const String &filename);
	int loadTerrains_LUA(const String &filename);
	//load troops
	bool loadTroops_XML(const String &filename);
	int loadTroops_LUA(const String &filename);
	List<String> corpCodes;
	TerrainCodeList terrainCodes;
	List<TroopCode> troopCodes;
	static string mapPath;

	//terrain edit
	bool getTerrain(uint x,uint y,Terrain &terrain)const;
	bool getTerrain(const Point &p,Terrain &terrain)const;
	bool setTerrain(uint x,uint y,const Terrain &terrain);
	bool setTerrain(const Point &p,const Terrain &terrain);
	bool fillTerrain(const Terrain &terrain);

	void autoAdjustTerrainsTiles();
	void autoAdjustTerrainTile(uint x,uint y);
	//variable
private:
	void saveUnitList(fstream &stream,const List<Unit> &unitList)const;
	void loadUnitList(fstream &stream,List<Unit> &unitList);
};
#endif // BATTLEFIELD_H