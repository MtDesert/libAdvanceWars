#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include"Corp.h"
#include"Terrain.h"
#include"Troop.h"
#include"Unit.h"

#include"ChessBoard.h"
#include"Set.h"

/*高级战争-战场
战场其实就是个矩形棋盘,主要用来容纳Terrain对象和Unit对象
除了游戏中起到了容器的作用外,还提供一些操作
*/
struct BattleField:public ChessBoard<Terrain,Unit>{
	BattleField();
	~BattleField();

	//读取地图
	int loadMap(const string &filename);
	bool loadMap_CSV(const string &filename);
	//保存地图
	int saveMap_CSV(const string &filename)const;
	void saveMap_CSV(FILE *file)const;
	//保存地图-BabyWars
	int saveMap_LUA_BabyWars(const string &filename)const;
	int saveMap_TMX_BabyWars(const string &filename)const;

	//外部数据表
	CorpsList *corpsList;//兵种码表,解读地图用
	TroopsList *troopsList;//部队码表,解读地图用
	TerrainCodesList *terrainsList;//地形码表,解读地图用

	//地图信息
	string mapName;//地图名
	string author;//地图作者
	//地形编辑
	bool getTerrain(uint x,uint y,Terrain &terrain)const;
	bool getTerrain(const Point2D<int> &p,Terrain &terrain)const;
	bool setTerrain(uint x,uint y,const Terrain &terrain);
	bool setTerrain(const Point2D<int> &p,const Terrain &terrain);
	bool setTerrain(uint x,uint y,const string &terrainName,const string &status="");
	bool fillTerrain(const Terrain &terrain);//地形填充
	//单位编辑
	bool addUnit(uint x,uint y,const string &corpName,const string &troopName);
	//图块调整
	void autoAdjustTerrainsTiles();//调整所有地图块的样式
	void autoAdjustTerrainTile(uint x,uint y);//调整x,y部分的样式,使其与周边看上去相连

	WhenErrorString whenError;//报错函数
	//地图分析,统计各个方面的数据
	void analyse();
	bool onlySea;//只有海
	int captureTerrainCount;//据点数
	Set<int> playerIndexList;//玩家索引表,依赖troopsList
};
#endif