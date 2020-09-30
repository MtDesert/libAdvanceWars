#include"Settings.h"
#include"Campaign.h"
#include"Directory.h"

CorpsList corpsList;
TroopsList troopsList;
TerrainCodesList terrainCodesList;
CommandersList commandersList;
WeathersList weathersList;

static void whenError(const string &err){
	printf("%s\n",err.data());
}

int main(int argc,char* argv[]){
	if(argc!=2)return -1;
	//获取参数
	auto filename=argv[1];
	//数据关联
	BattleField battleField;
	battleField.corpsList=&corpsList;
	battleField.troopsList=&troopsList;
	battleField.terrainsList=&terrainCodesList;
	battleField.whenError=whenError;
	//读取配置
	AwSettings settings;
	settings.loadFile("settings.lua",whenError);
	corpsList.loadFile_lua(settings.dataCorps,whenError);
	troopsList.loadFile_lua(settings.dataTroops,whenError);
	terrainCodesList.loadFile_lua(settings.dataTerrainCodes,whenError);
	//读取地图
	Directory::scan(filename,nullptr,[&](const string &name){
		printf("%s\n",name.data());
		if(battleField.loadMap_CSV(name))battleField.saveMap_CSV(name);
	});
	//battleField.loadMap_CSV(filename);
	//battleField.saveMap_CSV(filename);
	return 0;
}