#ifndef TERRAIN_H
#define TERRAIN_H

#include"DataList.h"

/*高级战争-地形
在高级战争的地图中,若一张图宽高为W,H,则这张图有W*H个地形,一张地图可以有若干重复的地形
*/
class Terrain
{
public:
	explicit Terrain(int type=0,int status=0);
	/*地形的类型(比如道路草地树林山地等)*/
	uint8 terrainType;
	/*一种地形可以有各种不同的状态
	比如道路,我们可以用来表示道路的方向
	比如据点,我们可以用来表示据点所属的势力
	*/
	uint8 status;
};

/*高级战争-地形代码
用于地形的查询,地形代码一般存在表中以供查询
地形代码主要记录的地形可能有的特征,比如地形有无方向性(比如道路就有),地形可否占领等
*/
struct TerrainCode{
	TerrainCode();

	string name;//地形名称
	uint8 defendLV;//地形防御等级
	bool capturable:1;//能否被占领,具有占领功能的兵种可以进行占领
	bool hasIncome:1;//是否具有收入,可占领的据点不一定会有收入
	bool buildable:1;//能否生产部队
	bool hidable:1;//是否可隐藏地形,隐藏地形只有靠近的时候才能发现内部情况
	bool has4direction:1;//地形是否具有4个方向,主要用于支持BattleField的调整图块功能
	string tileType;//图块类型,比如"水域""道路"等,主要用于支持BattleField的调整图块功能
};

//地形代码表
struct TerrainsList:public DataList<TerrainCode>{
	string loadFile_lua(const string &filename);//加载lua格式的数据文件,返回错误信息(返回nullptr表示无错误)
	bool canAdjustTile(int terrainType)const;//能否调整图块
	bool canAdjustTile(int terrainTypeA,int terrainTypeB)const;
};
#endif