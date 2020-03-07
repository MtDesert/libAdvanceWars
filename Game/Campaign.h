#ifndef CAMPAIGN_H
#define CAMPAIGN_H

#include"BattleField.h"
#include"LuaState.h"

//一场比赛中兵种可能用到的菜单项,可根据需求自行添加
#define CAMPAIGN_CORPMENU(macro)\
	macro(Join)\
	macro(Load)\
	macro(Drop)\
	macro(Fire)\
	macro(Capture)\
	macro(Supply)\
	macro(Build)\
	macro(Flare)\
	macro(Hide)\
	macro(Appear)\
	macro(Launch)\
	macro(Repair)\
	macro(Explode)\
	macro(Wait)

struct CampaignCO{//在参赛队伍中的CO
	CampaignCO();

	SizeType coID;//指挥官的ID
	SizeType energy;//指挥官所累计的能量
	SizeType powerStatus;//能力发动状态
};
struct CampaignTroop{//参赛队伍
	CampaignTroop();

	SizeType troopID;//部队id,具体对应什么部队请参考battleField->troopsList
	bool isAI;//指明队伍的是人脑还是电脑控制的
	SizeType teamID;//分组情况
	Array<SizeType> friendsTeams;//保存队伍的id,即把哪些参赛队伍当队友(注意:对应的参赛队伍未必会把你当队友),把对方视为队友时,自己的部队不会阻挡对方部队的移动
	Array<SizeType> enemiesTeams;//保存敌对方的id,即把哪些参赛队伍当敌人,只有把对方当敌人的情况下才能攻击对方

	//指挥官相关
	Array<CampaignCO> allCOs;//队伍中的所有CO
	int funds;//队伍的资金,可用于生产单位和修复单位
};
//比赛时候的天气
struct CampaignWeathers{
	struct Data{SizeType factor;double rate;};
	Array<Data> weatherData;//各个天气的发生因数

	void setWeatherAmount(SizeType amount);//设置天气数
	void setWeatherFactor(SizeType index,SizeType value);
};

struct CampaignRule{//比赛规则
	CampaignRule();

	bool mIsFogWar;//是否雾战状态
	int mInitFunds;//初始资金
	int mBaseIncome;//每个据点的收入
	int mBasesToWin;//达到多少据点算胜利,0忽略
	int mFundsToWin;//达到多少资金算胜利,0忽略
	int mTurnsToWin;//达到多少天算胜利,0忽略
	int mUnitLevel;//单位可升的等级,0不可升级
	int mCoPowerLevel;//CO可用的能力等级,0不可使用能力
};

/*一场比赛,除了关联战场信息之外,还保存有玩家信息
主要负责根据规则来设置数据状态
*/
class Campaign{
public:
	Campaign();
	~Campaign();

	typedef decltype(Unit::coordinate) CoordType;//坐标类型

	//战场数据
	BattleField *battleField;//战场地图(包括地图所用的数据表)
	Array<CampaignTroop> allTroops;//所有参赛队伍
	CampaignTroop* findTeam(SizeType teamID)const;
	CampaignTroop *currentTeam,*currentViewer;//当前行动队伍,当前观察者队伍
	CampaignWeathers campaignWeathers;//天气
	CampaignRule campaignRule;//规则

	//战前准备
	void makeAllTeams();//根据battleField自动生成所有队伍
	SizeType getCampaignTeamAmount()const;//根据battleField推断出需要的队伍数量

	//选择操作
	CoordType cursor;//光标
	void setCursor(const CoordType &p);
	void setCursor(int x,int y);//设置光标坐标,并获取相关信息
	//推导数据
	Terrain cursorTerrain;//光标指向的地形
	TerrainCode *cursorTerrainCode;//光标指向的地形码
	Unit *cursorUnit;//光标指向的单位
	Corp *cursorCorp;//光标指向的兵种
	CampaignTroop *cursorTeam;//光标指向的参赛队伍

	Unit *selectUnit;//所选单位
	Corp *selectCorp;//所选兵种
	CampaignTroop *selectTeam;//所选参赛队伍

	//范围表示
	struct MovePoint:public CoordType{//此结构体专门用于移动范围的计算
		uint8 remainMovement,remainFuel;//剩余移动力,剩余燃料
	};
	Array<MovePoint> movablePoints;//移动范围
	Array<MovePoint> movePath;//移动路径
	Array<CoordType> visiblePoints;//可视范围
	Array<CoordType> firablePoints;//可开火范围

	list<Unit*> suppliableUnits;//可补给的单位
	list<Unit*> dropableUnits;//可卸载的单位

	//菜单相关
	#define CAMPAIGN_ENUM(name) Menu_##name,
	enum EnumCorpMenu{//给每个兵种指令创建枚举
		CAMPAIGN_CORPMENU(CAMPAIGN_ENUM)
		AmountOfCorpEnumMenu
	};
	#undef CAMPAIGN_ENUM
	Array<int> corpMenu;//兵种的指令菜单,参考EnumMenu
	Array<int> produceMenu;//单位生产菜单,表中存放兵种索引

	bool isEditMode;//是否编辑模式
	//操作输入
	void cursorSelect();//在光标处执行选择操作
	void cursorCancel();//在光标处进行取消操作
	void executeMenuSelect(int index);//执行选择的菜单项
	void executeCorpMenu(int index);//执行兵种菜单项
	void executeProduceMenu(int index);//执行生产菜单项

	//lua相关
	LuaState luaState;//lua状态机,执行特定的规则代码
private:
	//lua函数
	int luaFunc_movementCost(const string &moveType,const string &terrainName,const string &weatherName);//查询移动损耗(移动类型,地形名,天气名),返回值小于0表示不可移动
	int luaFunc_fuelCost(const string &weatherName);//查询燃料损耗(天气名),返回值小于0表示不存在
	int luaFunc_loadableAmount(const string &loaderCorpName,const string &beLoaderCorpName)const;//查询部队可装载数量,小于等于0表示不可装载

	void selectCursorUnit();//选择光标处的单位,并获取相关信息
	//移动范围计算
	void caculateMovement(const Unit &unit,const Corp &corp);//计算unit的移动范围
	void tryToMoveTo(const MovePoint &currentPos,const CoordType &offset);
	//移动路径计算
	bool selectPath(const CoordType &p);
	bool tryToPathTo(const MovePoint &target);
	bool tryToBackPathTo(const MovePoint &current,const CoordType &offset);
	//移动执行
	bool moveWithPath();

	//火力范围计算
	void caculateFlightshot_byMovement();//根据移动范围来计算射程
	void caculateFlightshot_byCenter();//以间接攻击部队的中心来计算攻击距离
	void caculateFlightshot_byCenter(const CoordType &center,int minDistance,int maxDistance);
	void caculateFlightshot_byCenter(const CoordType &center,int distance);
	void caculateFlightshot_removeNotInRange();

	//菜单相关
	void showCorpMenu();
	void hideCorpMenu();
	//给每个兵种指令设定对应的处理函数
	#define CAMPAIGN_FUNC(name)\
	bool showMenuItem_##name();\
	bool execMenuItem_##name();
	CAMPAIGN_CORPMENU(CAMPAIGN_FUNC)
	#undef CAMPAIGN_FUNC

	//报错函数
	WhenErrorString whenError;
};
#endif