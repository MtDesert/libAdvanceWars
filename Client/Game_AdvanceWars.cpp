#include"Game_AdvanceWars.h"

#include"BattleField.h"
#include"extern.h"

#include"Scene_Main.h"
#include"Scene_BattleField.h"
#include"Layer_Conversation.h"

//所有场景
#define ALL_SCENES(MACRO)\
MACRO(Main)\
MACRO(BattleField)\
//场景声明
#define STATIC_SCENES(name) static Scene_##name *scene##name=nullptr;\

ALL_SCENES(STATIC_SCENES)

Game_AdvanceWars::Game_AdvanceWars(){
	gameSettings=&settings;
	//战场数据源
	battleField.corpsList=&mCorpsList;
	battleField.troopsList=&mTroopsList;
	battleField.terrainsList=&mTerrainsList;
}
Game_AdvanceWars::~Game_AdvanceWars(){
	//删除场景
	deleteSubObject(sceneMain);
	//清除数据
	mCorpsList.clear();
	mCommandersList.clear();
	mTerrainsList.clear();
	mTroopsList.clear();
	mWeathersList.clear();
	//清除纹理
	corpsTextures.clearCache();
	commandersHeadTextures.clearCache();
	commandersBodyTextures.clearCache();
	terrainsTextures.clearCache();
}

Game* Game::newGame(){
	game=new Game_AdvanceWars();
	return game;
}
Game_AdvanceWars* Game_AdvanceWars::currentGame(){
	return dynamic_cast<Game_AdvanceWars*>(game);
}
string Game_AdvanceWars::gameName()const{return"AdvanceWars";}

static void gotoSceneMain(){
	Game_AdvanceWars::currentGame()->gotoScene_Main();
}
void Game_AdvanceWars::reset(){
	settings.loadFile("settings.lua");//读取配置
	loadTranslationFile(settings.language+".csv");//读取翻译文件
	//进入场景
	/*auto scene=gotoScene_Logo();
	scene->reset();
	scene->logoText.setString("AdvanceWars_LifeTime",true);
	scene->whenLogoOver=gotoSceneMain;*/
	gotoSceneMain();
}

GAME_GOTOSCENE_DEFINE(AdvanceWars,Main)
GAME_GOTOSCENE_DEFINE(AdvanceWars,BattleField)

//数据表加载过程
/*#define GAME_DATA_LIST(Name) \
string Game_AdvanceWars::load##Name##List(bool forceReload){\
	string ret;\
	if(forceReload){\
		m##Name##List.clear();\
	}\
	if(m##Name##List.size()<=0){\
		m##Name##List.loadFile_lua(settings.Filename##Name);\
	}\
	return ret;\
}

GAME_DATA_LIST(Corps)//兵种表
GAME_DATA_LIST(Commanders)//指挥官资料表
GAME_DATA_LIST(Troops)//部队表
GAME_DATA_LIST(Terrains)//地形表

#undef GAME_DATA_LIST
*/

void Game_AdvanceWars::clearAllTextureCache(){
	corpsTextures.clearCache();
	commandersHeadTextures.clearCache();
	commandersBodyTextures.clearCache();
	terrainsTextures.clearCache();
}

void Game_AdvanceWars::loadCorpsTextures(bool forceReload){
	if(forceReload){
		corpsTextures.clearCache();
	}
}
void Game_AdvanceWars::loadCorpsTextures(const TroopsList &mTroopsList,bool forceReload){
	if(forceReload){
		corpsTextures.clearCache();
	}
}
void Game_AdvanceWars::loadCommandersTextures(bool forceReload){
	if(forceReload){
		commandersHeadTextures.clearCache();
	}
}
void Game_AdvanceWars::loadTerrainsTextures(const TerrainsList &mTerrainsList,bool forceReload){
	if(forceReload){
		terrainsTextures.clearCache();
	}
}