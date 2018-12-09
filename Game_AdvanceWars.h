#ifndef GAME_ADVANCEWARS_H
#define GAME_ADVANCEWARS_H

#include"Settings.h"
#include"Corp.h"
#include"Commander.h"
#include"Troop.h"
#include"Terrain.h"
#include"Weather.h"
#include"Campaign.h"
#include"Senario.h"

#include"Game.h"
#include"Texture.h"
//#include"GameScene_FileList.h"

/*高级战争的Game本体,负责管理场景的调度,即管理怎么从一个场景跳到另一个场景
除此之外,还负责管理一些基础数据和纹理
*/
class Game_AdvanceWars:public Game{
public:
	//构造/析构函数
	Game_AdvanceWars();
	~Game_AdvanceWars();

	static Game_AdvanceWars* currentGame();
	virtual string gameName()const;//游戏的名字

	enum FileType{//加载特定数据文件用,指明文件存放的是什么数据
		File_Corps,//兵种资料
		File_COs,//指挥官资料
		File_Troops,//部队资料
		File_Terrains,//地形资料
		File_Weathers,//天气资料
		File_DamageCaculator,//损伤计算器
		File_BattleField,//战场文件
		AmountOf_FileType
	};

	//override
	virtual void reset();
	virtual void render()const;
	//提示信息
	void showGameDialog(const string &content);
	void hideGameDialog();

	//场景跳转
	string gotoScene_FileData(FileType type,const string &filename);//选择好文件后,跳转到选择文件的场景,返回错误信息
	string gotoScene_BattleField(const string &filename);//根据文件名跳转到对应的战场场景中,返回错误信息
	bool gotoScene_CommanderInfo(uint index);//显示CO信息的场景
	bool gotoScene_Settings();
	//控制变量
	FileType currentFileType;//当前文件类型,用于调用特定的打开方式处理文件

	virtual void consumeTimeSlice();//处理文件列表场景的选择结果,并进入响应的资料显示场景

	//资料数据区
	Settings settings;//设置数据,游戏应该先读入设置数据
	static const char* translate(const string &english);//翻译(英文原文),返回译文,翻译失败则返回原文

	//资料表及其加载过程
#define GAME_DATA_LIST(Name) \
	Name##List m##Name##List;\
	string load##Name##List(bool forceReload=false);

	GAME_DATA_LIST(Corps)//兵种数据表
	GAME_DATA_LIST(Commanders)//指挥官资料表
	GAME_DATA_LIST(Troops)//部队表
	GAME_DATA_LIST(TerrainCode)//地形码表
	GAME_DATA_LIST(Weathers)//天气表
#undef GAME_DATA_LIST

	//游戏数据区
	BattleField battleField;//地图数据,负责存放地形和单位
	Campaign campaign;//竞赛,除了关联地图外,还包括了玩家信息以及规则设定
	Senario senario;//剧情

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
	void loadTerrainsTextures(const TerrainCodeList &terrainsList,bool forceReload=false);
};
#endif
