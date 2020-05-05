#ifndef AI_H
#define AI_H

#include"Campaign.h"
#include"Thread.h"

typedef Campaign::CoordType CoordType;

//目标单位信息
struct AI_TargetUnit{
	AI_TargetUnit(Unit *unit=nullptr);

	Unit *unit;//目标单位
	int manhattanDistance;//与目标单位的直角距离
	double distance;//与目标单位的直线距离
};

struct TroopAI;
/*单位AI,基于特定单位来搜集数据*/
struct UnitAI{
	UnitAI(Unit *unit=nullptr,TroopAI *troopAI=nullptr);

	Unit *unit;//单位本身
	TroopAI *troopAI;//上级AI

	enum Strategy{//单位行动策略
		Wait,//待机,什么都不做
		Move,//移动,移动到特定地点
		Follow,//跟随,跟踪特定单位
		NegativeDefend,//消极防御
		PositiveDefend,//积极防御
		Normal,//随机应变,根据情况行动
		NegativeAttack,//消极进攻,在原地待命,当敌人进入射程的时候发动攻击
		PositiveAttack,//积极进攻,主动寻找敌人,并且在自身损耗不大的情况下发动攻击
		SuicideAttack,//自杀式攻击,直接冲向目标,不畏惧敌人的火力
		AmountOfAllStrategy
	};
	Strategy strategy;

	Array<AI_TargetUnit> allAttackableTargets;//可攻击的目标
	CoordType targetCoord;//目标点
	AI_TargetUnit *targetUnit;//目标单位
	//输出
	CoordType moveCoord;//目标点,表示单位将会往这个点
	Campaign::EnumCorpMenu command;//命令,单位即将执行此命令
	CoordType commandCoord;//执行命令的坐标

	void getAttackableUnits();//获取可攻击的单位列表
	void selectAttackableUnit();//选择合适的目标
	void caculateMovement();//计算移动范围,并选择合适的点

	void startThinking();
};

/*部队AI,基于部队去思考战略情况*/
struct TroopAI{
	TroopAI();
	//情报数据
	Campaign *campaign;
	CampaignTroop *campaignTroop;//部队情报
	Array<UnitAI> allSelfUnitsAI,allEnemyUnitsAI;//所有自军单位AI,所有敌军单位AI
	Array<Campaign::CoordType> allCapturableBases;//所有可占领的据点

	void getIntelligence();//获取情报
	void analysis();//分析数据,并且给各个单位安排行动策略
	void predict();//预测
	void decision();//决策

	//输出
	UnitAI* getMovingUnitAI()const;//获取需要行动的单位AI
};

/*高级战争-人工智能*/
class AI{
	Thread aiThread;
public:
	AI();
	~AI();

	typedef decltype(Unit::coordinate) CoordType;

	Campaign campaign;//要思考的战局
	TroopAI troopAI;//部队思考模块
	//设置数据
	void setCampaign(const Campaign &orgCampaign);//设置当前战局,复制数据用于模拟

	//思考
	bool startThinking();//开始思考
	bool isThinking()const;//是否处于思考状态
};
#endif//AI_H