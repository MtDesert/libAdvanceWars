#include"Game_AdvanceWars.h"

#include"BattleField.h"
#include"extern.h"

#include"Scene_Main.h"
#include"Scene_BattleField.h"
#include"Layer_Conversation.h"

//场景
#define ALL_SCENES(MACRO)\
MACRO(Main)\
MACRO(BattleField)
//图层
#define ALL_LAYERS(MACRO)\
MACRO(Conversation)

#define STATIC_SCENES(name) static Scene_##name *scene##name=nullptr;
#define STATIC_LAYERS(name) static Layer_##name *layer##name=nullptr;
ALL_SCENES(STATIC_SCENES)
ALL_LAYERS(STATIC_LAYERS)

#define GOTO_SCENE(name)\
if(!scene##name){\
	scene##name=new Scene_##name();\
	gotoScene(scene##name);\
}

#define SHOW_LAYER(name)\
if(!layer##name){\
	layer##name=new Layer_##name();\
	Game_AdvanceWars::currentGame()->addSubObject(layer##name);\
}

Game_AdvanceWars::Game_AdvanceWars():senarioScript(nullptr){
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
	//删除脚本
	if(senarioScript)delete senarioScript;
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
string Game_AdvanceWars::gameName()const{return"AdvanceWars";}

void Game_AdvanceWars::reset(){
	//读取配置
	settings.loadFile("settings.lua",showDialogMessage);
	auto client=Game_AdvanceWars::currentClient();
	client->serverIPaddress=&settings.serverAddress;
	client->serverPort=&settings.serverPort;
	//读取翻译文件
	loadTranslationFile(settings.language+".csv");
	//重启场景
	GOTO_SCENE(Main)
	sceneMain->reset();
}

Game_AdvanceWars *Game_AdvanceWars::currentGame(){
	return dynamic_cast<Game_AdvanceWars*>(game);
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
	if(!sceneBattleField){
		sceneBattleField=new Scene_BattleField();
		subObjects.push_front(sceneBattleField);
		//设置各种属性
		sceneBattleField->battleField=&battleField;
		sceneBattleField->campaign=&campaign;
		sceneBattleField->terrainsTextures=&terrainsTextures;
		sceneBattleField->corpsTextures=&corpsTextures;
	}
	return battleField.loadMap_CSV(filename);
}
bool Game_AdvanceWars::gotoScene_CommanderInfo(uint index){
	/*scene_CommanderInfo.setCoInfo(index);
	subObjects.push_front(&scene_CommanderInfo);*/
	return true;
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

void Game_AdvanceWars::scriptInit(){
	if(senarioScript)return;
	//开始初始化,注册所有可能要用的函数
	senarioScript=new LuaState();
	senarioScript->whenError=showDialogMessage;
#define REGISTER_FUNCTION(name) senarioScript->registerFunction(#name,name);
	ALL_SENARIO_SCRIPTS(REGISTER_FUNCTION)
#undef REGISTER_FUNCTION
}
void Game_AdvanceWars::loadSenarioScript(const string &filename){
	//初始化脚本环境,并载入脚本
	scriptInit();
	if(!senarioScript->loadFile(filename))return;
	//移除主场景,准备开始
	clearAllScenes();
	senarioScript->protectCall();
}
//脚本函数
int Game_AdvanceWars::say(lua_State *state){
	if(lua_isstring(state,1)){
		SHOW_LAYER(Conversation)
		layerConversation->setDialogText(lua_tostring(state,1));
	}
	return 0;
}
int Game_AdvanceWars::bodySay(lua_State *state){
	if(lua_isstring(state,1) && lua_isstring(state,2)){
		printf("body: %s\n",lua_tostring(state,1));
		printf("say: %s\n",lua_tostring(state,2));
	}
	return 0;
}