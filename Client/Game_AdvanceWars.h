#ifndef GAME_ADVANCEWARS_H
#define GAME_ADVANCEWARS_H

#include"Settings.h"
#include"Commander.h"
#include"Weather.h"
#include"Campaign.h"
#include"DamageCaculator.h"
#include"ScenarioScript.h"

#include"Game.h"
#include"Scene_Main.h"
#include"Scene_BattleField.h"
#include"Scene_CampaignPrepare.h"
#include"Scene_BattleAnimation.h"
#include"Dialog_NewMap.h"
#include"Layer_Conversation.h"

#define GAME_AW auto game=Game_AdvanceWars::currentGame();

//所有场景
#define ALL_SCENES(MACRO)\
MACRO(Main)\
MACRO(BattleField)\
MACRO(CampaignPrepare)\
MACRO(BattleAnimation)

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

	//启动
	virtual void restart();

	//场景跳转
#define SCENE_DECL(Name) Scene_##Name* gotoScene_##Name(bool reset=false);
	ALL_SCENES(SCENE_DECL)
#undef SCENE_DECL
	//对话框显示
	Dialog_NewMap* showDialog_NewMap();

	//资料数据区
	Settings settings;//设置数据,游戏应该先读入设置数据

	//资料表及其加载过程
#define GAME_DATA_LIST(Name) \
	Name##List m##Name##List;\

	GAME_DATA_LIST(Corps)//兵种数据表
	GAME_DATA_LIST(Commanders)//指挥官资料表
	GAME_DATA_LIST(Troops)//部队表
	GAME_DATA_LIST(TerrainCodes)//地形码表
	GAME_DATA_LIST(Weathers)//天气表
#undef GAME_DATA_LIST
	bool loadAllConfigData();//加载配置数据,这些数据在游戏过程中不应该被改变
	bool loadAllTextures();//加载所有要用到的纹理

	//游戏数据区
	LuaState luaState;
	BattleField battleField;//地图数据,负责存放地形和单位
	Campaign campaign;//比赛数据,关于比赛过程产生的数据均放于此
	DamageCaculator damageCaculator;//损伤计算器,负责计算损伤

	//纹理缓冲区
	TextureCache commandersHeadTextures;//指挥官头像
	TextureCache commandersBodyTextures;//指挥官全身像
	TextureCache troopsTextures;//部队标记
	TextureCacheArray terrainsTexturesArray;//地形纹理的容器,用于支持快速查询
	TextureCacheArray corpsIconsArray;//兵种纹理的容器,用于支持快速查询
	TextureCacheArray corpsImagesArray;//兵种图片
	TextureCache_String allIconsTextures;//所有的菜单图标
	TextureCache mapEditMenuTextures;//地图编辑菜单纹理
	TextureCache campaignMenuTextures;//战役主菜单纹理
	TextureCache corpMenuTextures;//兵种菜单
	TextureCache numbersTextures;//数字,用于显示HP
	void clearAllTextureCache();

	//纹理加载过程
	void loadCorpsTextures(TextureCacheArray &corpsTexArray,const string &imagePath,bool forceReload=false);//读取兵种纹理
	void loadCommandersTextures(bool forceReload=false);//读取指挥官
	void loadTroopsTextures(bool forceReload=false);//读取部队纹理
	void loadTerrainsTextures(bool forceReload=false);//读取地形纹理
	void loadAllIconsTextures(bool forceReload=false);//加载所有图标,主要提供给菜单使用
	//纹理引用复制过程
	void loadMapEditMenuTextures(bool forceReload=false);//地图编辑菜单
	void laodCampaignMenuTextures(bool forceReload=false);//战役
	void loadCorpMenuTextures(bool forceReload=false);//兵种命令菜单

	//剧情脚本
	GAME_USE_CONVERSATION(Layer_Conversation)
	GAME_USE_SCRIPT(ScenarioScript)

	void notDone();//提示尚未制作
};
#endif