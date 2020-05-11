#include "AI.h"

AI_TargetUnit::AI_TargetUnit(Unit *unit):unit(unit),manhattanDistance(0),distance(0){}
UnitAI::UnitAI(Unit *unit,TroopAI *troopAI):unit(unit),troopAI(troopAI),strategy(Wait){}
TroopAI::TroopAI():campaign(nullptr){}
AI::AI(){}
AI::~AI(){}

void UnitAI::getAttackableUnits(){
	UnitData selfUd(troopAI->campaign),enemyUD(troopAI->campaign);
	selfUd.getUnitData(*this->unit);
	//添加敌方部队
	allAttackableTargets.clear();
	troopAI->allEnemyUnitsAI.forEach([&](UnitAI &unitAI){
		enemyUD.getUnitData(*unitAI.unit);
		//判断一下损伤表,有损伤值表示可以攻击
		auto dmg=troopAI->campaign->damageCaculator->predictDamage(selfUd,enemyUD,0,false);
		if(dmg>=0){
			allAttackableTargets.push_back(AI_TargetUnit(unitAI.unit));
		}
	});
}
void UnitAI::selectAttackableUnit(){
	//计算与每个目标的距离
	allAttackableTargets.forEach([&](AI_TargetUnit &target){
		auto delta=this->unit->coordinate-target.unit->coordinate;
		target.manhattanDistance=delta.manhattanLength();//直角距离
		target.distance=delta.distance();//直线距离
	});
	//选择一个比较进的
	targetUnit=nullptr;
	auto minDistance=INT_MAX;
	allAttackableTargets.forEach([&](AI_TargetUnit &target){
		auto sum = target.manhattanDistance + target.distance;
		if(sum < minDistance){//找到了更近的单位
			minDistance = sum;
			targetCoord=target.unit->coordinate;
			targetUnit=&target;
		}
	});
}

void UnitAI::caculateMovement(){
	moveCoord=unit->coordinate;
	//获取移动范围
	auto cmpgn=troopAI->campaign;
	cmpgn->clearAllOperation();
	cmpgn->setCursor(unit->coordinate);//选择自己
	cmpgn->cursorConfirm();//计算移动范围
	//寻找最近的点
	auto minDistance=INT_MAX;
	cmpgn->movablePoints.forEach([&](const CoordType &coord){
		if(cmpgn->battleField->getUnit(coord))return;//去掉有单位的点
		//计算距离
		auto delta=targetCoord-coord;
		auto sum = delta.manhattanLength() + delta.distance();
		if(sum < minDistance){//向更进的点前进
			minDistance = sum;
			moveCoord = coord;
		}
	});
	//尝试移动过去,看看能执行什么指令
	cmpgn->setCursor(moveCoord);
	cmpgn->cursorConfirm();//得到命令菜单
	//确定要执行的命令
	command=Campaign::Menu_Wait;
	commandCoord=moveCoord;
	//开火命令
	if(cmpgn->corpMenu.contain(Campaign::Menu_Fire) && cmpgn->targetPoints.size()){
		command=Campaign::Menu_Fire;
		commandCoord=*cmpgn->targetPoints.data(0);
	}
}

void UnitAI::startThinking(){
	switch(strategy){//根据策略进行思考
		case Wait:break;
		case Move:break;
		case Follow:break;
		case NegativeDefend:break;
		case NegativeAttack:break;
		case Normal:break;
		case PositiveDefend:break;
		case PositiveAttack://积极进攻
			getAttackableUnits();//寻找目标
			selectAttackableUnit();//选择目标
			caculateMovement();//计算移动
		break;
		case SuicideAttack:break;
		default:;
	}
}

void TroopAI::getIntelligence(){
	//获取单位信息
	allSelfUnitsAI.clear();
	allEnemyUnitsAI.clear();
	campaign->battleField->forEachUnit([&](Unit &unit){
		if(campaignTroop->troopID==unit.color){//添加自军单位
			allSelfUnitsAI.push_back(UnitAI(&unit,this));
		}else if(!campaign->isFriendUnit(*campaignTroop,unit)){//添加敌军单位
			allEnemyUnitsAI.push_back(UnitAI(&unit,this));
		}
	});
}
void TroopAI::analysis(){
	//全体积极进攻
	allSelfUnitsAI.forEach([](UnitAI &unitAI,SizeType){
		unitAI.strategy=UnitAI::PositiveAttack;
	});
}
void TroopAI::predict(){
	allSelfUnitsAI.forEach([](UnitAI &unitAI,SizeType){
		unitAI.startThinking();
	});
}
void TroopAI::decision(){}
UnitAI* TroopAI::getMovingUnitAI()const{
	return allSelfUnitsAI.data([](const UnitAI &unitAI){//未待机状态的部队开始行动
		return !unitAI.unit->isWait;
	});
}

//线程函数
static void* aiThinking(void *arg){
	auto ai=reinterpret_cast<AI*>(arg);
	auto &troopAI=ai->troopAI;
	//部队开始思考
	troopAI.getIntelligence();//获取所有的情报
	troopAI.analysis();//分析,制定单位的行动策略
	troopAI.predict();//预测
	troopAI.decision();//决策
	return nullptr;
}

void AI::setCampaign(const Campaign &orgCampaign){
	campaign=orgCampaign;
	troopAI.campaign=&campaign;
	troopAI.campaignTroop=campaign.currentTroop();
}

bool AI::startThinking(){
	return aiThread.start(aiThinking,this);
}
bool AI::isThinking()const{return aiThread.isRunning();}