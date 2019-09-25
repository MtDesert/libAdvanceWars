#include"Game_AdvanceWars.h"

#include"BattleField.h"
#include"extern.h"

#include"Scene_Main.h"
#include"Scene_BattleField.h"

extern string errorString;
//场景
static Scene_Main *sceneMain=nullptr;
static Scene_BattleField *scene_BattleField=nullptr;//战场场景,用来显示战场内容

Game_AdvanceWars::Game_AdvanceWars(){
	//读取配置
	/*
	//设定表格数据源
	scene_DataTable.tableCorpData.source=&mCorpsList;
	scene_DataTable.tableCorpData.textures=&corpsTextures;
	scene_DataTable.tableCommandersData.setSourceList(mCommandersList);
	scene_DataTable.tableCommandersData.textures=&commandersHeadTextures;
	//CO信息数据源
	scene_CommanderInfo.source=&mCommandersList;
	scene_CommanderInfo.bodyTextures=&commandersBodyTextures;*/
	//战场数据源
	battleField.corpsList=&mCorpsList;
	battleField.troopsList=&mTroopsList;
	battleField.terrainsList=&mTerrainsList;
	campaign.battleField=&battleField;
}
Game_AdvanceWars::~Game_AdvanceWars(){
	//删除场景
	removeSubObject(sceneMain);
	delete sceneMain;
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
string Game_AdvanceWars::gameName()const{return Game::translate("AdvanceWars");}

#define ASSERT(code)\
ok=code;\
if(!ok){\
printf("%s\n",errorString.data());\
return;\
}

void Game_AdvanceWars::reset(){
	bool ok;
	//读取配置
	ASSERT(settings.loadFile("settings.lua"))
	//读取翻译文件
	ASSERT(loadTranslationFile(settings.language+".csv"))
	//重启场景
	if(!sceneMain){
		sceneMain=new Scene_Main();
		addSubObject(sceneMain);
	}
	sceneMain->reset();
}
void Game_AdvanceWars::render()const{Game::render();}

Game_AdvanceWars *Game_AdvanceWars::currentGame(){
	return dynamic_cast<Game_AdvanceWars*>(game);
}

string Game_AdvanceWars::gotoScene_FileData(FileType type,const string &filename){
	string errStr;
	switch(type){
		case File_Corps:{//加载兵种数据和纹理
			//errStr=loadCorpsList();
			loadCorpsTextures(true);
		}break;
		case File_COs:{
			//errStr=loadCommandersList();
			loadCommandersTextures(true);
		}break;
		case File_Troops:{
			//errStr=loadTroopsList();
		}break;
		case File_Terrains:{
			//errStr=loadTerrainCodeList();
		}break;
		case File_Weathers:
			//errMsg=loadWeathersList();
		break;
		default:;
	}
	//无错误,可以显示场景
	return errStr;
}

#define GAME_ERRMSG(code) \
auto errMsg=code;\
if(errMsg)return errMsg;

string Game_AdvanceWars::gotoScene_BattleField(const string &filename){
	printf("加载地图%s\n",filename.data());
	loadCorpsList();//加载兵种数据
	loadTroopsList();//加载部队数据
	loadTerrainsList();//加载地形数据
	//生成纹理
	corpsTextures.clearCache();
	terrainsTextures.clearCache();
	loadCorpsTextures(mTroopsList);
	loadTerrainsTextures(mTerrainsList);
	//可以加载地图了
	if(!scene_BattleField){
		scene_BattleField=new Scene_BattleField();
		subObjects.push_front(scene_BattleField);
		//设置各种属性
		scene_BattleField->battleField=&battleField;
		scene_BattleField->campaign=&campaign;
		scene_BattleField->senario=&senario;
		scene_BattleField->terrainsTextures=&terrainsTextures;
		scene_BattleField->corpsTextures=&corpsTextures;
	}
	return battleField.loadMap_CSV(filename);
}
bool Game_AdvanceWars::gotoScene_CommanderInfo(uint index){
	/*scene_CommanderInfo.setCoInfo(index);
	subObjects.push_front(&scene_CommanderInfo);*/
	return true;
}
bool Game_AdvanceWars::gotoScene_Settings(){
	return true;
}

void Game_AdvanceWars::consumeTimeSlice(){
	//处理场景事件
	/*auto scene=subObjects.front();
	if(scene==&scene_FileList){//处理文件打开的过程
		if(scene_FileList.gameTable_Dir.menuStatus==GameMenuStatus::Confirm){
			if(scene_FileList.openFilename.length()){//选定了文件而非目录
				scene_FileList.gameTable_Dir.menuStatus=GameMenuStatus::Selecting;
				//读取文件过程有可能出错,我们需要得到错误信息并告诉玩家
				const char* errMsg=nullptr;
				if(currentFileType==File_BattleField){
					errMsg=gotoScene_BattleField(scene_FileList.openFilename);
				}else{
					errMsg=gotoScene_FileData(currentFileType,scene_FileList.openFilename);
				}
				//处理可能的错误情况
				if(errMsg){
					//弹出错误提示窗口
				}
			}
		}else if(scene_FileList.gameTable_Dir.menuStatus==GameMenuStatus::Cancel){
			scene_FileList.openFilename.clear();
			scene_FileList.reset();
			removeSubObject(&scene_FileList);
		}
	}else if(scene==&scene_BattleField){
		if(scene_BattleField.color.alpha==0){//debug
			scene_BattleField.color.alpha=255;
			removeSubObject(&scene_BattleField);
		}
	}else if(scene==&scene_DataTable){
		switch(scene_DataTable.tableCorpData.menuStatus){
			case GameMenu::Confirm:case GameMenu::Cancel:
				removeSubObject(scene);
			break;
			default:;
		}
	}*/
}

//数据表加载过程
#define GAME_DATA_LIST(Name) \
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
	/*int index=0;
	for(auto &item:mCorpsList){
		Texture tex;
		tex.texImage2D_FileBMP(settings.imagePathCorps+"/"+item.name+".bmp");
		corpsTextures.insert(index,tex);
		++index;
	}*/
}
void Game_AdvanceWars::loadCorpsTextures(const TroopsList &mTroopsList,bool forceReload){
	if(forceReload){
		corpsTextures.clearCache();
	}
	/*int index=0;
	for(auto &corp:mCorpsList){
		FileBMP fileBmp;
		fileBmp.loadFile(settings.imagePathCorps+"/"+corp.name+".bmp");
		fileBmp.parseData();
		int idx=0;
		for(auto &troop:mTroopsList){
			Texture tex;
			for(uint i=0;i<4;++i){
				fileBmp.bgrasList.setColor(i+6,troop.colors[i]);
			}
			tex.texImage2D(fileBmp);
			corpsTextures.insert(idx*256+index,tex);
			++idx;
		}
		fileBmp.deleteDataPointer();
		++index;
	}*/
}
void Game_AdvanceWars::loadCommandersTextures(bool forceReload){
	if(forceReload){
		commandersHeadTextures.clearCache();
	}
	/*int index=0;
	for(auto &co:mCommandersList){
		Texture tex0,tex1;
		tex0.texImage2D_FilePNG(settings.imagePathCommanders+"/"+co.coName+".png");
		commandersHeadTextures.insert(index,tex0);
		tex1.texImage2D_FilePNG(settings.imagePathCommandersBodies+"/"+co.coName+".png");
		commandersBodyTextures.insert(index,tex1);
		++index;
	}*/
}
void Game_AdvanceWars::loadTerrainsTextures(const TerrainsList &mTerrainsList,bool forceReload){
	if(forceReload){
		terrainsTextures.clearCache();
	}
	//int index=0;
	for(auto &item:mTerrainsList){
		printf("%s\n",item.name.data());
		/*if(item.capturable){//据点
			FileBMP fileBmp;
			fileBmp.loadFile(settings.imagePathTerrainCode+"/"+item.name+".bmp");
			fileBmp.parseData();
			int idx=0;
			for(auto &troop:mTroopsList){
				Texture tex;
				for(uint i=0;i<4;++i){
					fileBmp.bgrasList.setColor(i+2,troop.colors[i]);
				}
				tex.texImage2D(fileBmp);
				terrainsTextures.insert(idx*256+index,tex);
				++idx;
			}
			fileBmp.deleteDataPointer();
		}else if(item.has4direction){//有4个方向的纹理
			char charNum[4];
			for(int i=0;i<16;++i){
				sprintf(charNum,"%X",i);
				Texture tex;
				tex.texImage2D_FilePNG(settings.imagePathTerrainCode+"/"+item.name+"s/"+item.name+"_"+charNum+".png");
				terrainsTextures.insert(i*256+index,tex);
			}
		}else{
			Texture tex;
			tex.texImage2D_FilePNG(settings.imagePathTerrainCode+"/"+item.name+".png");
			terrainsTextures.insert(index,tex);
		}
		++index;*/
	}
}