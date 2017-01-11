#include "Terrain.h"

Terrain::Terrain(int type, int status)
{
	this->terrainType=type;
	this->status=status;
}
void Terrain::status_BitOr(uchar uch)
{
	status|=uch;
}
TerrainCode::TerrainCode():style(0){}

TerrainCode* TerrainCodeList::findTerrainCode(const String &name)const{
	return nullptr;
}
bool TerrainCodeList::canAdjustTile(_List_const_iterator<TerrainCode> itrA,
									_List_const_iterator<TerrainCode> itrB)const{
	if(itrA==end()||itrB==end())return false;
	//found terrain
	if(itrA->tileType=="Way" && itrB->style==2){return true;}//road/bridge & capturable
	if(itrA->tileType == itrB->tileType){return true;}//same type
	if(itrA->tileType=="Water" && itrB->name=="Bridge"){return true;}//special...
	if(itrA->name=="Bridge" && itrB->tileType!="Water"){return true;}//special...
	return false;
}