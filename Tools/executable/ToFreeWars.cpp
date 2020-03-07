#include"BattleField.h"
#include"AwbwCategory.h"

#include<list>
using namespace std;
#include<stdio.h>

#include"lua.hpp"

CorpsList corpsList;
TroopsList troopsList;
TerrainCodesList terrainsList;
BattleField battleField;
AwbwCategory category;

struct MapFile{
	string filename,mapname;
};
list<MapFile> mapList;

int systemRet=0;//专门接收system()返回值
#define system(arg) systemRet=::system(arg);//应对-Werror编译选项而改写

//判断地图有没有名字,并移动到特定文件夹
void throwNoNameMap(const string &filename){}
//添加地图到地图表中,可以用于比对
void addToMapList(const string &filename){}
//两两比对地图
void compareMapList(){
	for(auto itrA=mapList.begin();itrA!=mapList.end();++itrA){
		auto itrB=itrA;++itrB;
		for(;itrB!=mapList.end();++itrB){
			if(itrA->mapname==itrB->mapname){
				printf("%s \"%s\" !=== %s \"%s\"\n",itrA->filename.data(),itrA->mapname.data(),itrB->filename.data(),itrB->mapname.data());
			}
		}
	}
}
//给地图起名字
void addMapName(const string &filename){}

int main(int argc,char* argv[]){
	//if(argc!=2)return -1;
	//初始化
	corpsList.loadFile_lua("datas/Corps/Corps-AWDS.lua");//写死了...
	troopsList.loadFile_lua("datas/Troops/Troops-AWDS.lua");//写死了...
	troopsList.loadFile_lua("datas/Troops/Troops-AWBW.lua");//写死了...
	terrainsList.loadFile_lua("datas/Terrains/Terrains-DIY.lua");//写死了...
	battleField.corpsList=&corpsList;
	battleField.troopsList=&troopsList;
	battleField.terrainsList=&terrainsList;
	//扫描特定路径下的文件
	category.loadFile("awbw/Categories/S_Rank.txt");
	char filename[256];
	for(auto &id:category.allMapsID){
		printf("%d\n",id);
		sprintf(filename,"awbw/CategoriedMapsCsv/%d.csv",id);
		battleField.loadMap_CSV(filename);
		battleField.analyseFeature();
		sprintf(filename,"awbw/S_Rank/%d.tmx",id);
		battleField.saveMap_TMX_BabyWars(filename);
	}
	//收尾工作
	battleField.deleteData();
	corpsList.clear();
	troopsList.clear();
	terrainsList.clear();
	return 0;
}