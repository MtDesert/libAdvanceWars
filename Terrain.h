#ifndef TERRAIN_H
#define TERRAIN_H

#include"typedef.h"
#include"String.h"
#include"List.h"
class Terrain
{
public:
	explicit Terrain(int type=0,int status=0);
	uchar terrainType;
	uchar status;//different terrains have defferent means
	void status_BitOr(uchar);
};
struct TerrainCode
{
	TerrainCode();
	string name;//appear in source code
	uchar style;//0.None 1.has4direction 2.capturable
	string tileType;
};
struct TerrainCodeList:public List<TerrainCode>{
	TerrainCode* findTerrainCode(const String &name)const;
	bool canAdjustTile(_List_const_iterator<TerrainCode> itrA,
					   _List_const_iterator<TerrainCode> itrB)const;
};

#endif // TERRAIN_H
