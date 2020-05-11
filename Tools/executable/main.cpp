#include"Settings.h"
#include"Campaign.h"

CorpsList corpsList;
TroopsList troopsList;
TerrainCodesList terrainCodesList;
CommandersList commandersList;
WeathersList weathersList;

static void whenError(const string &err){
	printf("%s\n",err.data());
}

int main(int argc,char* argv[]){
	if(argc!=4)return -1;
	//获取参数
	auto coName=argv[1];
	auto terrainName=argv[2];
	auto weatherName=argv[3];
	//数据关联
	LuaState luaState;
	BattleField battleField;
	battleField.corpsList=&corpsList;
	battleField.troopsList=&troopsList;
	battleField.terrainsList=&terrainCodesList;
	Campaign campaign;
	campaign.battleField=&battleField;
	campaign.weathersList=&weathersList;
	campaign.luaState=&luaState;
	//读取配置
	AwSettings settings;
	settings.loadFile("settings.lua",whenError);
	corpsList.loadFile_lua(settings.dataCorps,whenError);
	troopsList.loadFile_lua(settings.dataTroops,whenError);
	terrainCodesList.loadFile_lua(settings.dataTerrainCodes,whenError);
	commandersList.loadFile_lua(settings.dataCommanders,whenError);
	weathersList.loadFile_lua(settings.dataWeathers,whenError);
	luaState.doFile(settings.ruleCommanders);
	//开始查询
	auto co=commandersList.data([&](const Commander &commander){return commander.name==coName;});
	auto terrainCode=terrainCodesList.data([&](const TerrainCode &code){return code.name==terrainName;});
	auto weather=weathersList.data([&](const Weather &weather){return weather.name==weatherName;});
	if(!co || !terrainCode || !weather){
		printf("error: %p %p %p\n",co,terrainCode,weather);
	}
	printf("co: %s\n",co->name.data());
	battleField.corpsList->forEach([&](const Corp &corp){
		printf("%s",corp.name.data());
		co->allPowers.forEach([&](const CommanderPower &power){
			auto feature=campaign.getCommanderPowerFeature(power.allFeatures,corp,*terrainCode,*weather);
			printf(",%d",feature.attack);
		});
		printf("\n");
	});
	return 0;
}