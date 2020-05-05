#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include"Corp.h"
#include"Terrain.h"
#include"Troop.h"
#include"Unit.h"

#include"ChessBoard.h"
#include"Set.h"

//高级战争-战场特征数据,用于保存分析后的结构,根据需求自行添加需要的内容
struct BattleField_Feature{
	void print()const;

	Array<SizeType> array_buildableTerrainAmount;//统计每个势力的可生产据点数(部队ID -> 据点数量)
	Array<SizeType> array_UnitAmount;//统计每个势力的单位数量(部队ID -> 单位数量)
};

/*高级战争-战场
战场其实就是个矩形棋盘,主要用来容纳Terrain对象和Unit对象
除了游戏中起到了容器的作用外,还提供一些操作
*/
struct BattleField:public ChessBoard<Terrain,Unit>{
	BattleField();
	~BattleField();

	typedef decltype(Unit::coordinate) CoordType;

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
	const CorpsList *corpsList;//兵种码表,解读地图用
	const TroopsList *troopsList;//部队码表,解读地图用
	const TerrainCodesList *terrainsList;//地形码表,解读地图用

	//地图信息
	string mapName;//地图名
	string author;//地图作者
	//地形编辑
	bool getTerrain(SizeType x,SizeType y,Terrain &terrain)const;
	bool getTerrain(const CoordType &p,Terrain &terrain)const;
	Terrain* getTerrain(SizeType x,SizeType y)const;
	Terrain* getTerrain(const CoordType &p)const;

	bool setTerrain(SizeType x,SizeType y,const Terrain &terrain);
	bool setTerrain(const CoordType &p,const Terrain &terrain);
	bool setTerrain(SizeType x,SizeType y,const string &terrainName,const string &status="");//根据名字和状态设置地形
	bool fillTerrain(const Terrain &terrain);//地形填充
	//添加单位
	Unit* addUnit(SizeType x,SizeType y,const string &corpName,const string &troopName);//x,y处添加兵种为corpName,部队为troopName的单位
	Unit* addUnit(SizeType x,SizeType y,SizeType corpID,SizeType troopID);//x,y处添加兵种ID为corpID,部队ID为troopID的单位
	Unit* addUnit(const Unit &unit);//添加具体单位
	//移除单位
	bool removeUnit(const CoordType &p);//移除p处的所有单位
	bool removeUnit(SizeType x,SizeType y);//移除x,y处的所有单位
	bool removeUnit(const Unit &unit);//移除具体单位
	//单位查询
	Unit* getUnit(SizeType x,SizeType y)const;
	Unit* getUnit(const CoordType &p)const;//获取p点处的单位
	Unit* getUnit(const Terrain &terrain)const;
	//单位遍历
	void forEachUnit(function<void(Unit &unit)> callback);
	//图块调整
	void autoAdjustTerrainsTiles();//调整所有地图块的样式
	void autoAdjustTerrainTile(SizeType x,SizeType y,bool adjustAround=false);//调整x,y部分的样式,使其与周边看上去相连,如果adjustAround为true,则周边的图块也会一起调整

	WhenErrorString whenError;//报错函数
	//地图分析,统计各个方面的数据
	void analyseFeature(BattleField_Feature &feature)const;
};
#endif