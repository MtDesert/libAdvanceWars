#include"Campaign.h"

//获取战役
static Campaign* getCampaignFromLuaState(lua_State *state){//获取lua中的campaign
	Campaign *ret=nullptr;
	if(lua_getglobal(state,"campaign")==LUA_TLIGHTUSERDATA){
		ret=reinterpret_cast<Campaign*>((void*)lua_topointer(state,-1));
		lua_pop(state,1);
	}
	return ret;
}
#define GET_CAMPAIGN \
auto campaign=getCampaignFromLuaState(state);\
if(!campaign)return 0;\
LuaState ls(state);

//遍历单位({self=自军处理函数,friend=友军处理函数,enemy=敌军处理函数})
static int forEachUnit(lua_State *state){//遍历自己的单位
	GET_CAMPAIGN
	if(lua_type(state,-1)!=LUA_TTABLE)return 0;
	//开始过滤自军部队
	auto troop=campaign->currentTroop();
	if(troop){
		const char *relation[]={"self","friend","enemy"},*name=nullptr;
		campaign->battleField->forEachUnit([&](Unit &unit){
			//确定关系
			if(campaign->isSelfUnit(*troop,unit)){//自军
				name=relation[0];
			}else if(campaign->isFriendUnit(*troop,unit)){//友军
				name=relation[1];
			}else{//敌军
				name=relation[2];
			}
			//获取相关函数
			if(ls.getTableFunction(name)){
				ls.push(&unit);
				ls.protectCall();
			}
		});
	}
	return 0;
}

//修复单位
static int repairUnit(lua_State *state){
	GET_CAMPAIGN
	const void *pUnit=nullptr;
	int repairHP=0,costPercent=100;
	ls.getLightUserData(1,pUnit);
	ls.getInteger(2,repairHP);
	ls.getInteger(3,costPercent);
	if(pUnit){//回复HP
		campaign->repairUnit(*((Unit*)pUnit),repairHP,costPercent);
	}
	return 0;
}
//降低单位HP
static int reduceUnitHP(lua_State *state){
	GET_CAMPAIGN
	const void *pUnit=nullptr;
	int reduceHP=0;
	ls.getLightUserData(1,pUnit);
	ls.getInteger(2,reduceHP);
	if(pUnit){//回复HP
		campaign->reduceUnitHP(*((Unit*)pUnit),reduceHP);
	}
	return 0;
}

//火箭的火力掩护(波及范围,损伤值)
static int rocketCoveringFire(lua_State *state){
	GET_CAMPAIGN
	//获取波及范围和威力
	auto troop=campaign->currentTroop();
	if(!troop)return 0;
	int range=0,damage=0;
	ls.getInteger(1,range);
	ls.getInteger(2,damage);
	//搜索敌人
	typedef Campaign::CoordType PtType;
	struct CoverPoint:public PtType{
		int priceLost;//可对敌方造成的造价损失
		CoverPoint(const PtType &p=PtType()):priceLost(0){setP(p);}
	};
	Array<CoverPoint> firePoints;//火力点
	campaign->battleField->forEachUnit([&](Unit &unit){
		if(!campaign->isEnemyUnit(*troop,unit))return;//只搜索敌人
		campaign->caculateRange(unit.coordinate,0,range,[&](const PtType &p){//记录下能波及到unit的点
			auto cp=firePoints.data([&](const CoverPoint &coverP){return coverP==p;});
			if(!cp){
				firePoints.push_back(CoverPoint(p));
			}
		});
	});
	if(firePoints.size()<=0)return 0;//没有敌人,不需要火力覆盖
	Array<CoverPoint> coveringPoints;//覆盖范围
	firePoints.forEach([&](CoverPoint &center){//计算每个火力点的覆盖范围
		center.priceLost=0;//开始评估火力点的得失情况
		campaign->caculateRange(center,0,range,[&](const PtType &p){//计算覆盖范围
			//搜索尚未存入的覆盖范围
			auto cp=coveringPoints.data([&](const CoverPoint &coverP){return coverP==p;});
			if(cp){
				center.priceLost += cp->priceLost;//累加结果
				return;
			}
			//查询单位,兵种
			auto unit=campaign->battleField->getUnit(p);
			if(!unit)return;
			auto corp=campaign->getCorp(*unit);
			if(!corp)return;
			//计算单位可能会损失的hp
			int hpLost = damage < unit->healthPower ? damage : unit->healthPower-1;//单位HP最小降为1
			CoverPoint coverP(p);
			coverP.priceLost = Number::divideRound(hpLost*corp->price,UNIT_MAX_HP);
			//要判断会不会对自己造成损失
			if(campaign->isFriendUnit(*troop,*unit)){
				coverP.priceLost*=-1;
			}
			coveringPoints.push_back(coverP);
			//累加结果
			center.priceLost += coverP.priceLost;
		});
	});
	//寻找让敌方价值损失最大的方案
	CoverPoint maxLost;
	maxLost.priceLost=INT_MIN;
	firePoints.forEach([&](const CoverPoint &cp){
		if(cp.priceLost > maxLost.priceLost){
			maxLost=cp;
		}
	});
	//开始掩护
	campaign->caculateRange(maxLost,0,range,[&](const PtType &p){
		auto unit=campaign->battleField->getUnit(p);
		if(unit){
			campaign->reduceUnitHP(*unit,damage);
		}
	});
	return 0;
}

#define REGISTER_FUNC(name) luaState->registerFunction(#name,::name);
void Campaign::registerDefaultScriptFunction(){
	REGISTER_FUNC(forEachUnit)
	REGISTER_FUNC(repairUnit)
	REGISTER_FUNC(reduceUnitHP)
	REGISTER_FUNC(rocketCoveringFire)
}