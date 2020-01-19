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
	battleField.terrainsList=&mTerrainCodesList;
	battleField.whenError=whenError;
}
Game_AdvanceWars::~Game_AdvanceWars(){
	//删除场景
	deleteSubObject(sceneMain);
	deleteSubObject(sceneBattleField);
	//清除数据
	mCorpsList.clear();
	mCommandersList.clear();
	mTerrainCodesList.clear();
	mTroopsList.clear();
	mWeathersList.clear();
	//清除纹理
	clearAllTextureCache();
}

Game* Game::newGame(){
	game=new Game_AdvanceWars();
	return game;
}
Game_AdvanceWars* Game_AdvanceWars::currentGame(){
	return dynamic_cast<Game_AdvanceWars*>(game);
}
string Game_AdvanceWars::gameName()const{return"AdvanceWars";}

void Game_AdvanceWars::reset(){
	settings.loadFile("settings.lua");//读取配置
	loadTranslationFile(settings.language+".csv");//读取翻译文件
	//进入场景
	/*auto scene=gotoScene_Logo();
	scene->reset();
	scene->logoText.setString("AdvanceWars_LifeTime",true);
	scene->whenLogoOver=[&](){gotoScene_Main();};*/
	gotoScene_Main();
}

GAME_GOTOSCENE_DEFINE(AdvanceWars,Main)
GAME_GOTOSCENE_DEFINE(AdvanceWars,BattleField)

#define AW_LOAD_LUA(mList,dataName) if(mList.size()==0)mList.loadFile_lua(settings.dataName,whenError);
bool Game_AdvanceWars::loadAllConfigData(){
	AW_LOAD_LUA(mTerrainCodesList,dataTerrainCodes)//地形表
	AW_LOAD_LUA(mCorpsList,dataCorps)//兵种表
	AW_LOAD_LUA(mTroopsList,dataTroops)//势力表
	return true;
}
bool Game_AdvanceWars::loadAllTextures(){
	loadTerrainsTextures();
	loadCorpsTextures(mTroopsList);
	return false;
}

void Game_AdvanceWars::clearAllTextureCache(){
	corpsTextures.clearCache();
	commandersHeadTextures.clearCache();
	commandersBodyTextures.clearCache();
	troopsTextures.clearCache();
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
void Game_AdvanceWars::loadTerrainsTextures(bool forceReload){
	//未加载,重新加载
	terrainsTexturesArray.setSize(mTerrainCodesList.size(),true);
	int trnIndex=0;
	for(auto &trnCode:mTerrainCodesList){
		auto texArr=terrainsTexturesArray.data(trnIndex);
		if(trnCode.capturable){//据点
			texArr->setSize(mTroopsList.size(),true);
			//开始用png调色
			FilePNG filePng;
			filePng.loadFile(settings.imagesPathTerrainCodes+"/"+trnCode.name+".png",whenError);
			//开始调色
			auto plte=filePng.findPLTE();
			if(plte){
				auto i=0;
				for(auto &troop:mTroopsList){//根据部队配色表来进行调色
					for(int i=0;i<4;++i){
						plte->setColor(2+i,troop.colors[i]);
					}
					//完成配色,生成纹理
					auto tex=texArr->data(i);
					tex->texImage2D(filePng);
				}
			}
			//调色完毕
			filePng.memoryFree();
		}else if(trnCode.has4direction){//有方向性的图块
			auto amount=16;
			char num[3];
			texArr->setSize(amount,true);
			for(int i=0;i<amount;++i){
				sprintf(num,"%X",i);
				auto tex=texArr->data(i);
				tex->texImage2D_FilePNG(settings.imagesPathTerrainCodes+"/"+trnCode.name+"s/"+trnCode.name+"_"+num+".png");
			}
		}else{//其他地形
			texArr->setSize(1,true);
			auto tex=texArr->data(0);
			tex->texImage2D_FilePNG(settings.imagesPathTerrainCodes+"/"+trnCode.name+".png");
		}
		//下一个
		++trnIndex;
	}
}