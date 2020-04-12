#ifndef CAMPAIGN_H
#define CAMPAIGN_H

#include"BattleField.h"
#include"Weather.h"
#include"DamageCaculator.h"
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

	//指挥官相关
	Array<CampaignCO> allCOs;//队伍中的所有CO
	int funds;//队伍的资金,可用于生产单位和修复单位

	bool isLose;//状态,表明此队伍是否已经输了
};
//比赛时候的天气
struct CampaignWeathers{
	struct Data{SizeType factor;double rate;};
	Array<Data> weatherData;//各个天气的发生因数

	void setWeatherAmount(SizeType amount);//设置天气数
	void setWeatherFactor(SizeType index,SizeType value);//设置天气发生因数
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

//单位相关数据
class Campaign;
struct UnitData{
	UnitData(Campaign *cmpgn=nullptr);

	Unit *unit;//单位本身
	const Corp *corp;//unit的对应兵种
	Terrain *terrain;//unit对应的地形
	const TerrainCode *terrainCode;//terrain对应的地形码
	CampaignTroop *campaignTroop;//unit所属的战役部队
	const Troop *troop;//所属的部队数据
	//数据源,依靠数据源的线索来查询数据
	Campaign *campaign;

	void getUnitData(const decltype(Unit::coordinate) &p,bool clearCache=true);//获取p处相关信息
	void getUnitData(const Unit &unit);//获取单位unit的相关信息
	void clear();

	//字符串
	string strUnit()const;//单位信息
};

class DamageCaculator;
/*一场战役,除了关联战场信息之外,还保存有玩家信息
主要负责根据规则来设置数据状态
*/
class Campaign{
	friend class DamageCaculator;
public:
	Campaign();
	~Campaign();

	typedef decltype(Unit::coordinate) CoordType;//坐标类型

	//战场,参战部队,天气变化规律,参赛规则
	BattleField *battleField;//战场地图(包括地图所用的数据表)
	WeathersList *weathesList;//天气数据表
	Array<CampaignTroop> allTroops;//所有参赛队伍
	CampaignWeathers campaignWeathers;//天气
	CampaignRule campaignRule;//规则
	DamageCaculator *damageCaculator;//损伤计算器

	//参赛部队
	CampaignTroop* findCampaignTroop(const Unit &unit)const;//通过单位寻找所属的CampaignTroop
	CampaignTroop* findCampaignTroop(const Terrain &terrain)const;//通过单位寻找所属的CampaignTroop
	SizeType currentTroopIndex;//当前参赛队伍索引
	SizeType currentWeatherIndex;//当前天气

	//战前准备
	void makeAllTeams();//根据battleField自动生成所有队伍
	void beginTurn();
	void endTurn();//结束当前回合
	void nextCampaignTroopTurn();//下一个部队开始行动

	//选择操作
	CoordType cursor;//光标
	void setCursor(const CoordType &p);//移动光标
	void cursorConfirm();//在光标处执行确认操作
	void cursorCancel();//在光标处进行取消操作
	void clearAllOperation();//清除所有操作
	UnitData cursorUnitData;//光标的实时数据

	//范围表示
	struct MovePoint:public CoordType{//此结构体专门用于移动范围的计算
		uint8 remainMovement,remainFuel;//剩余移动力,剩余燃料
	};
	struct FlarePoint:public CoordType{
		uint8 troopID,flareRange;//发射闪光弹的部队,闪光范围
	};
	Array<MovePoint> movablePoints;//移动范围
	Array<MovePoint> movePath;//移动路径
	Array<CoordType> firablePoints;//可开火范围
	Array<FlarePoint> flarePoints;//闪光点,即闪光弹所在的点
	Array<CoordType> impactScope;//波及范围

	CoordType *selectedTargetPoint;//需要选择的目标点
	bool selectedTargetPointFreely;//自由选择目标点,直接移动光标选择,而不是从候选列表中自动选择

	//菜单相关
	#define CAMPAIGN_ENUM(name) Menu_##name,
	enum EnumCorpMenu{//给每个兵种指令创建枚举
		CAMPAIGN_CORPMENU(CAMPAIGN_ENUM)
		AmountOfCorpEnumMenu
	};
	#undef CAMPAIGN_ENUM
	Array<EnumCorpMenu> corpMenu;//兵种的指令菜单,显示当前单位可用的命令,参考EnumMenu
	EnumCorpMenu corpMenuCommand;//当前菜单命令

	Array<Unit*> unitsArray;//单位列表,可供选择用
	Array<int> produceMenu;//单位生产菜单,表中存放兵种索引

	//菜单命令执行
	void executeCorpMenu(int command);//执行兵种菜单项
	void executeProduceMenu(int index);//执行生产菜单项
	//目标选择
	void choosePrevTarget();//选择上一个目标
	void chooseTarget(int delta);
	void chooseNextTarget();//选择下一个目标
	//卸载单位
	bool selectDropPoint(Unit &unit);//选择unit的卸载点,返回能否选择

	//关系判定
	bool isFriendUnit(const CampaignTroop &campTroop,const Unit &unit)const;//判断unit是不是campTroop的友军
	bool isFriendTerrain(const CampaignTroop &campTroop,const Terrain &terrain)const;//判断terrain是不是campTroop的友军地形

	//lua相关
	LuaState luaState;//lua状态机,执行特定的规则代码
private:
	//单位数据操作
	UnitData selectedUnitData;//选择的状态数据
	//目标状态数据
	Array<CoordType> targetPoints;//扫描出来的目标点
	Array<Unit*> suppliableUnits;//可补给的单位
	//lua函数
	int luaFunc_movementCost(const string &moveType,const string &terrainName,const string &weatherName);//查询移动损耗(移动类型,地形名,天气名),返回值小于0表示不可移动
	int luaFunc_fuelCost(const string &weatherName);//查询燃料损耗(天气名),返回值小于0表示不存在
	int luaFunc_volumnOfLoader(const string &loaderName);
	int luaFunc_unitWeight(const string &loaderName,const string &beLoaderName,const string &beLoaderCorpType);//查询部队可装载数量,小于等于0表示不可装载
	string luaFunc_buildTerrain(const string &terrain);

	//移动范围计算
	Array<int> movementCostCache;//缓冲(地形编号=>移动损耗)
	Weather *currentWeather;//当前天气,用于帮助计算
	void caculateMovement(const Unit &unit,const Corp &corp);//计算unit的移动范围
	void tryToMoveTo(const MovePoint &currentPos,const CoordType &offset);//尝试以currentPos开始移动offset个单位
	//移动路径计算
	bool selectPath(const CoordType &p);//选择路径(目标点为p),返回路径是否发生变化
	bool tryToPathTo(const MovePoint &target);//试图正常走到目标点,这个点必须和路径的最后一个点相邻(否则返回false),返回能否移动过去
	bool tryToBackPathTo(const MovePoint &current,const CoordType &offset);//从current位置往offset方向寻找能通往起点的路径,返回能否找到
	//卸载部队计算
	Unit *unitToDrop;//要卸载的部队
	bool canStayAt(const string &moveType,const CoordType &p,bool checkBarrier);//判断moveType的兵种能否停在战场位置p上,checkBarrier为false时不检查p点处已经存在的单位
	bool canDropFrom(const Unit &unit, const CoordType &p,bool needSave=false);//判断unit能否从落点p卸载,若needSave则会保存到targetPoints中
	//移动执行
	bool moveWithPath();

	//范围计算
	void caculateRange(const CoordType &center,int distance,function<void (const CoordType &)> callback);
	void caculateRange(const CoordType &center,int minDistance,int maxDistance,function<void(const CoordType &p)> callback);
	//视野计算
	void caculateVision();
	void caculateVision(const Unit &unit);
	//火力范围计算
	void caculateFlightshot_byMovement();//根据移动范围来计算射程
	void caculateFlightshot_byCenter();//以间接攻击部队的中心来计算攻击距离
	void caculateFlightshot_byCenter(const CoordType &center,int minDistance,int maxDistance);

	//补给修复计算
	void caculateSuppliableUnits();//寻找出可补给的单位
	void supplyUnit(Unit &unit);//对unit进行补给
	void repairUnit(Unit &unit,int repairHP,int repairPayPercent=100);//对unit进行修复,hp回复量为repairHP,修理费百分比为repairPayPercent
	void reduceUnitHP(Unit &unit,int reduceHP);//减少unit的HP,但最低保持1

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