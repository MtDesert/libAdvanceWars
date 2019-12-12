#ifndef GAME_ADVANCEWARS_H
#define GAME_ADVANCEWARS_H

#include"Settings.h"
#include"Corp.h"
#include"Commander.h"
#include"Troop.h"
#include"Terrain.h"
#include"Weather.h"
#include"BattleField.h"
#include"ScenarioScript.h"

#include"Game.h"
#include"Scene_Main.h"
#include"Scene_BattleField.h"
#include"Layer_Conversation.h"

/*高级战争的Game本体,负责管理场景的调度,即管理怎么从一个场景跳到另一个场景
除此之外,还负责管理一些基础数据和纹理,以及脚本的调度
*/
class Game_AdvanceWars:public Game{
public:
	//构造/析构函数
	Game_AdvanceWars();
	~Game_AdvanceWars();

	static Game_AdvanceWars* currentGame();
	string gameName()const;

	//override
	virtual void reset();

	//场景跳转
	Scene_Main* gotoScene_Main(bool reset=false);
	Scene_BattleField* gotoScene_BattleField(bool reset=false);

	//资料数据区
	Settings settings;//设置数据,游戏应该先读入设置数据

	//资料表及其加载过程
#define GAME_DATA_LIST(Name) \
	Name##List m##Name##List;\
	string load##Name##List(bool forceReload=false);

	GAME_DATA_LIST(Corps)//兵种数据表
	GAME_DATA_LIST(Commanders)//指挥官资料表
	GAME_DATA_LIST(Troops)//部队表
	GAME_DATA_LIST(Terrains)//地形码表
	GAME_DATA_LIST(Weathers)//天气表
#undef GAME_DATA_LIST

	//游戏数据区
	BattleField battleField;//地图数据,负责存放地形和单位

	//纹理缓冲区
	TextureCache corpsTextures;//兵种纹理(数量为兵种数*势力数)
	TextureCache commandersHeadTextures;//指挥官头像纹理
	TextureCache commandersBodyTextures;//指挥官全身纹理
	TextureCache terrainsTextures;//地形纹理
	void clearAllTextureCache();

	//纹理加载过程
	void loadCorpsTextures(bool forceReload=false);//读取兵种纹理,主要用于生成资料表
	void loadCorpsTextures(const TroopsList &troopsList,bool forceReload=false);//读取兵种纹理,战场地图用
	void loadCommandersTextures(bool forceReload=false);
	void loadTerrainsTextures(const TerrainsList &terrainsList,bool forceReload=false);
	//剧情脚本
	GAME_USE_CONVERSATION(Layer_Conversation)
	GAME_USE_SCRIPT(ScenarioScript)
};
#endif