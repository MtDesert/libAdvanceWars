#include"Campaign.h"
#include"DamageCaculator.h"
#include"Number.h"
#include"StringList.h"
#include"PrintF.h"
#include"define.h"

CampaignCO::CampaignCO():coID(0),energy(0),powerLevel(0),onUnit(nullptr){}
CampaignTroop::CampaignTroop():troopID(0),isAI(false),teamID(0),funds(0),isLose(false){}
CampaignTroop::TerrainAmount::TerrainAmount():terrainID(0xFF),amount(0){}
CampaignRule::CampaignRule():mIsFogWar(false),mInitFunds(0),mBaseIncome(1000),
mBasesToWin(0),mFundsToWin(0),mTurnsToWin(0),mUnitLevel(0),mCoPowerLevel(2),
//核心规则
baseRepairHP(20),captureProgressMax(20),buildProgressMax(20),unitRepairHP(10),launchImpactRange(2),launchImpactDamage(30),unitExplodeRange(3),unitExplodeDamage(50),attackFixWhenCOpower(10),defenceFixWhenCOpower(10),
//特殊规则
allowLoadCOonUnit(false),allowBaseRepairFriendUnit(false),allowLoadUnitOnFriendUnit(false),allowSupplyFriendUnit(false),allowUnitRepairFriendUnit(false){}
UnitData::UnitData(Campaign *cmpgn):campaign(cmpgn){clear();}

decltype(Commander::allPowers)* CampaignCO::getCOpowersList(const Campaign &campaign)const{
	auto commander=campaign.commandersList->data(coID);
	return commander ? &commander->allPowers : nullptr;
}
void CampaignCO::changeCoPower(Campaign &campaign,SizeType level){
	powerLevel=level;
	//执行瞬发技能
	auto powersList=getCOpowersList(campaign);
	if(!powersList)return;
	auto power=powersList->data(level);
	if(!power)return;
	//执行所有瞬间发动的技能
	campaign.luaState->setGlobalLightUserData("campaign",&campaign);//让技能能从战役中获取数据
	power->allFeatures.forEach([&](const CommanderPowerFeature &cpf){
		if(cpf.executeFunction.empty())return;
		auto state=campaign.luaState;
		//开始调用函数
		if(!state->getGlobalFunction(cpf.executeFunction))return;
		state->protectCall();
	});
	campaign.luaState->clearStack();
}
CampaignCO* CampaignTroop::currentGlobalCO()const{
	for(auto &co:allCOs){
		if(!co.onUnit)return &co;
	}
	return nullptr;
}
void CampaignTroop::updateTerrainsInfo(const BattleField &battleField){
	allCapturableTerrains.clear();
	battleField.forEachLattice([&](uint,uint,const Terrain &terrain){
		auto code=battleField.terrainsList->data(terrain.terrainType);
		if(!code)return;
		//累计数量
		auto pTerrainAmount=allCapturableTerrains.data([&](const TerrainAmount &ta){return ta.terrainID==terrain.terrainType;});
		if(pTerrainAmount){//找到了,直接添加
			++pTerrainAmount->amount;
		}else{//找不到,添加数据
			allCapturableTerrains.push_back(TerrainAmount());
			pTerrainAmount=allCapturableTerrains.lastData();
			++pTerrainAmount->amount;
		}
	});
	allCapturableTerrains.sort([](const TerrainAmount &a,const TerrainAmount &b){return a.terrainID<b.terrainID;});
}

void UnitData::getUnitData(const decltype(Unit::coordinate) &p,bool clearCache){
	if(!campaign)return;
	if(clearCache)clear();
	//获取地形信息
	auto field = campaign->battleField;
	terrain = field->getTerrain(p.x,p.y);
	if(terrain)terrainCode=campaign->getTerrainCode(*terrain);
	//获取单位信息
	if(!unit){
		unit = field->getUnit(p);
	}
	if(unit)corp=campaign->getCorp(*unit);
	//获取所属部队信息
	campaignTroop = unit ? campaign->findCampaignTroop(*unit) : nullptr;
	troop = campaignTroop ? campaign->battleField->troopsList->data(campaignTroop->troopID) : nullptr;
	//获取指挥官信息
	campaignCO = unit ? campaign->getCampaignCO(*unit) : nullptr;
	commander = campaignCO ? campaign->commandersList->data(campaignCO->coID) : nullptr;
}
void UnitData::getUnitData(const Unit &unit){
	clear();
	this->unit=(Unit*)&unit;
	getUnitData(unit.coordinate,false);
}
void UnitData::clear(){
	unit=nullptr;
	corp=nullptr;
	terrain=nullptr;
	terrainCode=nullptr;
	campaignTroop=nullptr;
	troop=nullptr;
	campaignCO=nullptr;
	commander=nullptr;
}

Campaign::Campaign():battleField(nullptr),damageCaculator(nullptr),
currentDay(0),currentTroopIndex(0),currentWeatherIndex(0),
selectedTargetPoint(nullptr),selectedTargetPointFreely(false),
corpMenuCommand(AmountOfCorpEnumMenu),
whenExecuteMoveUnit(nullptr),currentWeather(nullptr),unitToDrop(nullptr),whenError(nullptr){
	cursorUnitData.campaign=this;
	selectedUnitData.campaign=this;
}
Campaign::~Campaign(){}

Campaign& Campaign::operator=(const Campaign &another){
	//创建缓冲
	if(!battleField)battleField=new BattleField();
	//复制数据
#define COPY(obj) obj=another.obj;
	*battleField=*another.battleField;//复制战场
	COPY(commandersList)//指挥官
	COPY(weathersList)//天气配置
	COPY(allTroops)//部队
	COPY(campaignWeathers)//复制天气发生概率
	COPY(campaignRule)//复制规则
	COPY(damageCaculator)//复用损伤计算器

	COPY(currentDay)//当前天数
	COPY(currentTroopIndex)//当前行动的部队
	COPY(currentWeatherIndex)//当前天气
	COPY(luaState)//依赖脚本也要传递
#undef COPY
	return *this;
}

const Corp* Campaign::getCorp(const Unit &unit)const{return battleField->corpsList->data(unit.corpType);}
const TerrainCode* Campaign::getTerrainCode(const Terrain &terrain)const{return battleField->terrainsList->data(terrain.terrainType);}

CampaignTroop* Campaign::currentTroop()const{return allTroops.data(currentTroopIndex);}
CampaignTroop* Campaign::findCampaignTroop(const Unit &unit)const{
	return allTroops.data([&](const CampaignTroop &troop){
		return unit.color==troop.troopID;
	});
}
CampaignTroop* Campaign::findCampaignTroop(const Terrain &terrain)const{
	auto terrainCode=battleField->terrainsList->data(terrain.terrainType);
	if(!terrainCode || !terrainCode->capturable)return nullptr;
	return allTroops.data([&](const CampaignTroop &troop){
		return terrain.status==troop.troopID;
	});
}

void CampaignWeathers::setWeatherAmount(SizeType amount){
	weatherData.setSize(amount,true);
	weatherData.fill({0,0});
}
void CampaignWeathers::setWeatherFactor(SizeType index,SizeType value){
	auto dt=weatherData.data(index);
	if(!dt)return;
	//设置数据,更新所有概率
	dt->factor=value;
	SizeType sum=0;
	for(auto &data:weatherData)sum += data.factor;//累加
	for(auto &data:weatherData){
		data.rate = sum>=0 ? data.factor/(double)sum : 0;
	}
}

void Campaign::makeAllTeams(){
	//根据地图的统计信息来找出可参战队伍
	BattleField_Feature feature;
	battleField->analyseFeature(feature);
	allTroops.clear();//清理参赛队伍,重新确定
	for(SizeType i=0;i<battleField->troopsList->size();++i){
		auto pBuildCount=feature.array_buildableTerrainAmount.data(i);
		auto pUnitCount=feature.array_UnitAmount.data(i);
		if(*pBuildCount + *pUnitCount > 0){//有厂或者有单位,就当作可以参战了
			auto teamID=allTroops.size();
			allTroops.push_back(CampaignTroop());
			auto troop=allTroops.lastData();
			//设置默认数据
			troop->troopID=i;
			troop->isAI=(allTroops.size()>1);
			troop->teamID=teamID;
		}
	}
}
void Campaign::beginTurn(){
	//清除掉自军的闪光弹的区域
	auto troop=currentTroop();
	if(troop){
		flarePoints.remove_if([&](const FlarePoint &fp){
			return fp.troopID==troop->troopID;
		});
	}
	//重新计算视野
	caculateVision();
}
void Campaign::currentTroop_GetIncome(){
	auto troop=currentTroop();
	if(troop){
		battleField->forEachLattice([&](uint x,uint y,Terrain &terrain){
			auto code=battleField->terrainsList->data(terrain.terrainType);
			if(code && code->hasIncome && terrain.status==troop->troopID){//自己占领的有收入的据点,可以获得收入
				troop->funds += campaignRule.mBaseIncome;
			}
		});
	}
}
void Campaign::currentTroop_AllBaseRepairAllUnit(){
	auto troop=currentTroop();
	if(troop){
		UnitData ud(this);
		bool canFix=false;
		battleField->forEachUnit([&](Unit &unit){
			if(unit.color==troop->troopID){//只修复自军的单位
				ud.getUnitData(unit);
				canFix = campaignRule.allowBaseRepairFriendUnit ?
					isFriendTerrain(*troop,*ud.terrain):
					isSelfTerrain(*troop,*ud.terrain);
				if(canFix && (ud.corp->corpType==ud.terrainCode->repairType||ud.corp->corpType==ud.terrainCode->produceType)){//修复类型必须和兵种类型一致
					repairUnit(unit,campaignRule.baseRepairHP);//按规定的修复量去修复单位
				}
			}
		});
	}
}
void Campaign::endTurn(){
	//清除待机状态(只清除当前部队的)
	auto troop=currentTroop();
	if(troop){
		battleField->forEachUnit([&](Unit &unit){
			if(unit.color==troop->troopID){
				unit.isWait=false;
			}
		});
	}
	//下一个队伍行动
	nextCampaignTroopTurn();
	beginTurn();
}
void Campaign::nextCampaignTroopTurn(){
	auto oldIndex=currentTroopIndex;
	do{
		//改变当前部队
		++currentTroopIndex;
		auto sz=allTroops.size();
		if(currentTroopIndex>=sz){//到新的一天
			currentTroopIndex=0;
			++currentDay;
		}
		//跳过已经输掉的部队
		auto troop=allTroops.data(currentTroopIndex);
		if(troop && !troop->isLose)break;//到没输的部队行动
	}while(currentTroopIndex!=oldIndex);//防止无限循环
}

void Campaign::setCursor(const CoordType &p){
	if(!battleField->isInRange(p.x,p.y))return;//过滤
	if(selectedTargetPoint && !selectedTargetPointFreely){//需要选择目标,这里要判断
		auto tp=targetPoints.data([&](const CoordType &tp){return tp==p;});
		if(tp){
			cursor=p;
			cursorUnitData.getUnitData(p);
		}
	}else{
		cursor=p;
		cursorUnitData.getUnitData(p);
		if(movablePoints.size() && corpMenuCommand>=AmountOfCorpEnumMenu){
			selectPath(p);
		}
	}
}

static const Campaign::CoordType directionP[]={{0,1},{0,-1},{-1,0},{1,0}};
//LUA交互
int Campaign::luaFunc_movementCost(const string &moveType,const string &terrainName,const Weather &weather){
	int ret=-1;
	luaState->doString("weather="+weather.toLuaString());//天气
	if(luaState->getGlobalFunction("movementCost")){
		luaState->push(moveType).push(terrainName);
		if(luaState->protectCall()){
			luaState->getTopInteger(ret);
		}
	}
	return ret;
}
int Campaign::luaFunc_fuelCost(const string &weatherName){
	int ret=-1;
	if(luaState->getGlobalFunction("fuelCost")){
		luaState->push(weatherName);
		if(luaState->protectCall()){
			luaState->getTopInteger(ret);
		}
	}
	return ret;
}
int Campaign::luaFunc_volumnOfLoader(const string &loaderName){
	int ret=-1;
	if(luaState->getGlobalFunction("volumnOfLoader")){
		luaState->push(loaderName);
		if(luaState->protectCall()){
			luaState->getTopInteger(ret);
		}
	}
	return ret;
}
int Campaign::luaFunc_unitWeight(const string &loaderName,const string &beLoaderName,const string &beLoaderCorpType){
	int ret=-1;
	if(luaState->getGlobalFunction("unitWeight")){
		luaState->push(loaderName).push(beLoaderName).push(beLoaderCorpType);
		if(luaState->protectCall()){
			luaState->getTopInteger(ret);
		}
	}
	return ret;
}
string Campaign::luaFunc_buildTerrain(const string &terrain){
	string ret;
	if(luaState->getGlobalFunction("getBuildTerrain")){
		luaState->push(terrain);
		if(luaState->protectCall()){
			ret=luaState->getTopString();
		}
	}
	return ret;
}

void Campaign::caculateMovement(const Unit &unit,const Corp &corp){
	//记录起点信息、剩余移动力、剩余燃料
	MovePoint mp;
	mp.setP(unit.coordinate);
	mp.remainMovement=corp.movement;
	mp.remainFuel=unit.fuel;
	//CO对移动力修正
	auto feature=getCommanderPowerFeature(selectedUnitData);
	mp.remainMovement += feature.movement;
	//清除缓冲
	movementCostCache.setSize(battleField->terrainsList->size(),true);
	for(auto &move:movementCostCache)move=-1;
	//开始对点进行推算
	movablePoints.clear();
	movablePoints.push_back(mp);
	movePath.clear();
	movePath.push_back(mp);
	//取天气,开始计算
	currentWeather=weathersList->data(currentWeatherIndex);
	if(!currentWeather)return;
	for(SizeType i=0;i<movablePoints.size();++i){
		auto p=*movablePoints.data(i);
		for(auto &dir:directionP){//尝试向周边移动
			tryToMoveTo(p,dir);
		}
	}
}

void Campaign::tryToMoveTo(const MovePoint &currentPos,const CoordType &offset){
	CoordType targetPos = currentPos + offset;
	//获取地形
	auto terrain=battleField->getTerrain(targetPos);
	if(!terrain)return;
	//获取地形信息，判断是否可移动的地形
	int moveCost=0;
	auto intPtr=movementCostCache.data(terrain->terrainType);//搜索缓冲
	if(intPtr && *intPtr>=0){//有缓冲结果,可直接用
		moveCost = *intPtr;
	}else{//无缓冲结果,查询地形表
		auto trnCode=battleField->terrainsList->data(terrain->terrainType);
		if(trnCode){
			moveCost=luaFunc_movementCost(selectedUnitData.corp->moveType,trnCode->name,*currentWeather);
			(*movementCostCache.data(terrain->terrainType))=moveCost;//缓存起来
			if(moveCost<0)return;
		}
	}
	//碰撞检测，检测是否发生碰撞
	auto unit=battleField->getUnit(*terrain);
	if(unit && unit->isVisible){
		if(!isFriendUnit(*selectedUnitData.campaignTroop,*unit))return;//判断是敌人还是朋友,不是朋友的一般都会采取拦截
	}
	//检查剩余的移动力和燃料
	if(currentPos.remainMovement < moveCost)return;
	//传递给lua来判断损耗
	int fuelCost=luaFunc_fuelCost(currentWeather->name);
	if(fuelCost<0 || currentPos.remainFuel < fuelCost)return;

	//创建新的移动点
	MovePoint p;
	p.setP(targetPos);
	p.remainMovement = currentPos.remainMovement - moveCost;
	p.remainFuel = currentPos.remainFuel - fuelCost;
	//检查是否重复
	auto exist=movablePoints.data([&](const MovePoint &mp){return mp==p;});
	if(exist){//更新现有移动点信息
		if(exist->remainMovement < p.remainMovement){
			*exist=p;
		}
	}else{//插入新的移动点
		movablePoints.push_back(p);
	}
}

/*添加路径上的点，这个函数有可能会被频繁调用
p为目标点，该函数会进行自动寻路
false表示路径没有变化，true表示路径发生了变化*/
bool Campaign::selectPath(const CoordType &p){
	//检查目标点在不在移动路径内
	for(SizeType i=0;i<movePath.size();++i){
		auto pathP=movePath.data(i);
		if(p==*pathP){//在路径内,那么需要判断路径是否需要发生变化
			if(i+1==movePath.size())return false;//没变化
			//删除多余路径
			movePath.setSize(i+1);
			return true;
		}
	}
	//--检查目标点在不在移动范围内
	for(auto &mp:movablePoints){
		if(p!=mp)continue;
		//试图移动到新的点
		if(tryToPathTo(mp))return false;
		else{//不能沿着新的路径移动过去，那么我们就调整路径
			movePath.clear();
			movePath.push_front(mp);
			auto pp = movePath.data(0);
			bool found=true;
			while(found){
				found=false;//防止while死循环
				for(auto &dir:directionP){
					if(tryToBackPathTo(*pp,dir)){//只要找到一个,那就改变pp的值
						pp=movePath.data(0);
						found=true;//继续找
						break;
					}
				}
			}
		}
		break;
	}
	return true;
}
bool Campaign::tryToPathTo(const MovePoint &target){
	if(movePath.size()<=0)return false;
	//这个点必须和路径的最后一个点是相邻的
	auto lastP=movePath.data(movePath.size()-1);
	CoordType delta=target-(*lastP);
	if(delta.manhattanLength()!=1)return false;
	//取地形
	if(!cursorUnitData.terrainCode)return false;
	//看看剩余的移动力能不能过去
	int moveCost=luaFunc_movementCost(selectedUnitData.corp->moveType,cursorUnitData.terrainCode->name,*currentWeather);
	if(lastP->remainMovement < moveCost)return false;
	//看看剩余的燃料能不能过去
	int costFuel=luaFunc_fuelCost(currentWeather->name);
	if(costFuel<0)return false;
	if(lastP->remainFuel < costFuel)return false;
	//可以移动过去
	auto p=target;
	p.remainMovement = lastP->remainMovement-moveCost;
	p.remainFuel = lastP->remainFuel-costFuel;
	movePath.push_back(p);
	return true;
}
bool Campaign::tryToBackPathTo(const MovePoint &current,const CoordType &offset){
	//搜索一下有没有临近点
	decltype(offset) target=current+offset;
	auto prev=movablePoints.begin();
	for(;prev!=movablePoints.end();++prev){
		if(*prev==target){
			if(prev->remainMovement <= current.remainMovement)return false;
			movePath.push_front(*prev);
			return true;
		}
	}
	return false;
}

Campaign::CoordType Campaign::movePath_SrcCoord()const{
	auto p=movePath.firstData();
	return p ? *p : selectedUnitData.unit->coordinate;
}
Campaign::CoordType Campaign::movePath_DstCoord()const{
	auto p=movePath.lastData();
	return p ? *p : selectedUnitData.unit->coordinate;
}

bool Campaign::canStayAt(const string &moveType,const CoordType &p,bool checkBarrier){
	UnitData unitData(this);
	unitData.getUnitData(p);
	if(!unitData.terrainCode)return false;
	int moveCost=luaFunc_movementCost(moveType,unitData.terrainCode->name,*currentWeather);
	if(moveCost<0)return false;//必须是可停靠地形
	if(checkBarrier){
		if(unitData.unit && unitData.unit->isVisible){//有障碍单位
			if(unitData.unit==selectedUnitData.unit){//是载体,说明载体移动过
				return true;
			}
			return false;
		}
	}
	return true;
}
bool Campaign::canDropFrom(const Unit &unit,const CoordType &p,bool needSave){
	auto corp=getCorp(unit);
	if(!corp)return false;
	if(canStayAt(corp->moveType,p,false)){//检测落点是否可走
		if(needSave)targetPoints.clear();
		for(auto &dir:directionP){
			if(canStayAt(corp->moveType,p + dir,true)){
				if(needSave)targetPoints.push_back(p+dir);
				else return true;
			}
		}
		if(needSave)return targetPoints.size()>0;
	}
	return false;
}

//计算范围
void Campaign::caculateRange(const CoordType &center,int distance,function<void(const CoordType &)> callback){
	int d=distance;
	for(int i=0;i<d;++i){
		callback(CoordType(center.x+i,center.y+(i-d)));
		callback(CoordType(center.x+(d-i),center.y+i));
		callback(CoordType(center.x-i,center.y+(d-i)));
		callback(CoordType(center.x+(i-d),center.y-i));
	}
}
void Campaign::caculateRange(const CoordType &center,int minDistance,int maxDistance,function<void(const CoordType&)> callback){
	for(int i=minDistance;i<=maxDistance;++i){//计算范围
		if(i>0){
			caculateRange(center,i,callback);
		}else{
			callback(center);
		}
	}
}
//计算视野
void Campaign::caculateVision(){
	if(!campaignRule.mIsFogWar){//雾战要重新计算视野
		return;
	}
	//先隐藏掉所有地形和单位
	battleField->forEachLattice([&](uint x,uint y,Terrain &terrain){
		terrain.isVisible=false;
		auto unit=battleField->getUnit(terrain);
		if(unit)unit->isVisible=false;
	});
	//寻找出观察者
	auto viewer=currentTroop();
	if(viewer){
		battleField->chessPieces.forEach([&](Unit &unit,SizeType){
			if(isFriendUnit(*viewer,unit)){//计算自军和友军的视野
				unit.isVisible=true;
				caculateVision(unit);
			}
		});
	}
	//这里还要计算闪光弹照明的视野
	flarePoints.forEach([&](FlarePoint &center,SizeType){
		caculateRange(center,0,center.flareRange,[&](const CoordType &p){
			auto trn=battleField->getTerrain(p);
			if(trn)trn->isVisible=true;
		});
	});
}
void Campaign::caculateVision(const Unit &unit){
	auto see=[&](Terrain *terrain){
		if(terrain){
			terrain->isVisible=true;
			auto u=battleField->getUnit(*terrain);
			if(u)u->isVisible=true;
		}
	};
	caculateRange(unit.coordinate,0,1,[&](const CoordType &p){//看见unit自己和周围1格的地形
		see(battleField->getTerrain(p));
	});
	auto corp=getCorp(unit);
	if(corp){
		caculateRange(unit.coordinate,2,corp->vision,[&](const CoordType &p){
			auto trn=battleField->getTerrain(p);
			if(trn){//只能看透非掩体地形
				auto code=getTerrainCode(*trn);
				if(code && !code->hidable){
					see(trn);
				}
			}
		});
	}
}

void Campaign::caculateFlightshot_byMovement(){//根据移动范围来计算射程
	firablePoints.push_back(selectedUnitData.unit->coordinate);
	for(SizeType i=0;i<movablePoints.size();++i){
		auto p=*movablePoints.data(i);
		for(auto &dir:directionP){
			auto pp=p+dir;
			if(battleField->isInRange(pp.x,pp.y) && !firablePoints.contain(pp)){
				firablePoints.push_back(pp);
			}
		}
	}
}
void Campaign::caculateFlightshot_byCenter(){
	firablePoints.clear();
	if(selectedUnitData.unit && selectedUnitData.corp){
		auto wpn=selectedUnitData.corp->firstAttackableWeapon();//取首个武器判断
		if(wpn && wpn->isIndirectAttack()){
			caculateFlightshot_byCenter(selectedUnitData.unit->coordinate,wpn->minRange,wpn->maxRange);
		}
	}
}
void Campaign::caculateFlightshot_byCenter(const CoordType &center,int minDistance,int maxDistance){
	caculateRange(center,minDistance,maxDistance,[&](const CoordType &p){
		if(battleField->isInRange(p.x,p.y))firablePoints.push_back(p);
	});
}

void Campaign::caculateSuppliableUnits(bool isRepair){
	suppliableUnits.clear();
	bool allowFriend = isRepair ? campaignRule.allowUnitRepairFriendUnit : campaignRule.allowSupplyFriendUnit;
	bool ok=false;
	auto func=[&](const CoordType &p){
		auto unit=battleField->getUnit(p);
		if(unit){//必须是自军或者友军
			ok = allowFriend?
				isFriendUnit(*selectedUnitData.campaignTroop,*unit):
				isSelfUnit(*selectedUnitData.campaignTroop,*unit);
			if(ok)suppliableUnits.push_back(unit);
		}
	};
	caculateRange(cursor,1,func);
}
void Campaign::supplyUnit(Unit &unit){
	auto corp=getCorp(unit);
	if(corp){
		unit.fuel=corp->gasMax;//补充燃料
		auto wpn=corp->firstAttackableWeapon();
		unit.ammunition = wpn ? wpn->ammunitionMax : 0;//补充弹药
	}
}
void Campaign::repairUnit(Unit &unit,int repairHP,int repairPayPercent){
	//找兵种,取其价格
	auto corp=getCorp(unit);
	if(!corp)return;
	//找部队,取其资金
	auto campaignTroop=findCampaignTroop(unit);
	if(!campaignTroop)return;
	//计算可修复量
	int maxRepairHP=INT_MAX;
	if(corp->price && repairPayPercent){//防止除数为0
		/*最大可修复量
		 *=金钱/((造价/最大HP) * (折扣百分数/100))
		 *=金钱/((造价*折扣百分数) / (最大HP*100))
		 *=(金钱*最大HP*100) / (造价*折扣百分数)
		*/
		maxRepairHP = (campaignTroop->funds*UNIT_MAX_HP*100) / (corp->price*repairPayPercent);
	}
	repairHP=min(repairHP,maxRepairHP);//根据金钱确定修复量
	repairHP=min(repairHP,UNIT_MAX_HP-unit.healthPower);//根据损伤程度确定修复量
	int repairCost = repairHP * corp->price / UNIT_MAX_HP;//修理费用=修复量*造价/最大HP
	//开始修理
	unit.healthPower += repairHP;//HP恢复
	campaignTroop->funds-=repairCost;//支付费用
}
void Campaign::reduceUnitHP(Unit &unit,int reduceHP){
	unit.healthPower = unit.healthPower>reduceHP ? unit.healthPower-reduceHP : 1;
}

void Campaign::cursorConfirm(){
	if(selectedUnitData.unit){
		if(selectedTargetPoint){//选定了目标,再次执行指令
			executeCorpMenu(corpMenuCommand);
		}else{//没有选定目标,尝试生成兵种命令菜单
			if(movePath_DstCoord()==cursor){//在移动范围内
				auto troop=currentTroop();
				if(troop && troop->troopID==selectedUnitData.unit->color){//有可能选择了别人的部队,这里要做检查
					if(!selectedUnitData.unit->isWait){//待机状态也要做检查
						showCorpMenu();//自己的未行动部队,可以显示菜单
					}
				}
			}else{//不在移动范围内,清除移动范围
				cursorCancel();
			}
		}
	}else{
		selectedUnitData=cursorUnitData;
		if(selectedUnitData.unit && selectedUnitData.corp){//如果光标处有单位,则计算移动范围
			caculateMovement(*selectedUnitData.unit,*selectedUnitData.corp);//计算移动范围
			if(selectedUnitData.corp->isDirectAttack()){//计算火力范围(直接攻击部队的情况下)
				caculateFlightshot_byMovement();
				for(auto &p:movablePoints){//移除和移动范围一样的点
					firablePoints.remove(p);
				}
			}
		}else{
			auto produceType=cursorUnitData.terrainCode->produceType;
			if(!produceType.empty()){//如果没有单位,则有可能是制造单位的生产厂
				auto troop=currentTroop();
				if(troop && troop->troopID==selectedUnitData.terrain->status){//是自军的生产厂
					produceMenu.clear();
					//生成生产菜单(后期可加入黑白名单)
					produceTroopID=troop->troopID;
					battleField->corpsList->forEach([&](const Corp &corp,SizeType idx){
						if(corp.corpType==produceType){
							produceMenu.push_back(idx);
						}
					});
				}
			}
		}
	}
}
void Campaign::cursorCancel(){
	if(selectedTargetPoint){//退回菜单模式
		selectedTargetPoint=nullptr;
		showCorpMenu();
		return;
	}
	if(selectedUnitData.unit){
		clearAllOperation();
	}else{
		selectedUnitData=cursorUnitData;
		auto selectUnit=selectedUnitData.unit;
		auto selectCorp=selectedUnitData.corp;
		if(selectUnit && selectCorp){//显示攻击范围
			if(selectCorp->isDirectAttack()){
				caculateMovement(*selectUnit,*selectCorp);
				caculateFlightshot_byMovement();
				movablePoints.clear();
			}
			if(selectCorp->isIndirectAttack()){
				caculateFlightshot_byCenter();
			}
		}
	}
}
void Campaign::clearAllOperation(){
	//清除范围
	movablePoints.clear();
	movePath.clear();
	firablePoints.clear();
	//清除菜单
	corpMenuCommand=AmountOfCorpEnumMenu;
	hideCorpMenu();
	produceMenu.clear();
	//清除刚操作的单位信息
	selectedUnitData.clear();
	cursorUnitData.getUnitData(cursor);
	//清除各种缓冲
	selectedTargetPoint=nullptr;
	selectedTargetPointFreely=false;
	unitsArray.clear();
	unitToDrop=nullptr;
}

bool Campaign::executeCorpMenu(int command){
	corpMenuCommand = (EnumCorpMenu)command;
	if(command>=AmountOfCorpEnumMenu)return true;
	//执行
	bool executeFinish=false;
	switch(command){
#define CASE(name) case Menu_##name:executeFinish=execMenuItem_##name();break;
		CAMPAIGN_CORPMENU(CASE)
#undef CASE
		default:;
	}
	//执行完毕就清除相关操作
	if(executeFinish){
		clearAllOperation();
		caculateVision();
		if(command==Menu_Drop){//卸载指令的情况下,可以继续执行卸载
			selectedUnitData=cursorUnitData;
			execMenuItem_Drop();
		}
	}
	return executeFinish;
}
void Campaign::executeProduceMenu(int index){
	//根据idx确定要执行的command
	auto corpID=produceMenu.data(index);
	if(corpID){
		auto unit=battleField->addUnit(cursor.x,cursor.y,*corpID,produceTroopID);//产生部队
		if(unit){unit->isWait=true;}//待机状态
	}
	//确定了兵种,我们可以进行生产
	clearAllOperation();
}

void Campaign::choosePrevTarget(){chooseTarget(-1);}
void Campaign::chooseTarget(int delta){
	if(!selectedTargetPoint)return;
	//计算索引
	auto idx=targetPoints.indexOf(*selectedTargetPoint);
	idx += delta;
	if(idx>=(int)targetPoints.size())idx=0;
	else if(idx<0)idx=targetPoints.size()-1;
	//更新指针
	selectedTargetPoint=targetPoints.data(idx);
	setCursor(*selectedTargetPoint);
}
void Campaign::chooseNextTarget(){chooseTarget(1);}

bool Campaign::selectDropPoint(Unit &unit){
	targetPoints.clear();
	canDropFrom(unit,movePath_DstCoord(),true);
	if(targetPoints.size()>0){
		unitToDrop=&unit;
		selectedTargetPoint=targetPoints.firstData();
		selectedTargetPointFreely=false;
		chooseTarget(0);
		hideCorpMenu();
		unitsArray.clear();//单位菜单消失
		return true;
	}
	return false;
}

CampaignCO* Campaign::getCampaignCO(const Unit &unit)const{
	auto troop=findCampaignTroop(unit);
	if(!troop)return nullptr;
	//搜索临场指挥官
	CampaignCO *campaignCO=nullptr;
	for(auto &co:troop->allCOs){
		if(!co.onUnit)continue;//临场指挥的co会搭载在单位上
		if(battleField->getUnit(co.onUnit->coordinate) != co.onUnit)continue;//搭载CO的单位必须存活在地图上
		//必须在其指挥范围内
		auto cmd=commandersList->data(co.coID);
		if(cmd){
			auto range = cmd->commandRange;
			auto dist=(unit.coordinate-co.onUnit->coordinate).manhattanLength();
			if(dist<=range){//在范围内,找到了
				campaignCO=&co;
				break;
			}
		}
	}
	//找不到临场指挥,则找队列中的总指挥
	if(!campaignCO)campaignCO=troop->currentGlobalCO();
	return campaignCO;
}
CampaignCO* Campaign::getCurrentGlobalCampaignCO()const{
	auto troop=currentTroop();
	if(!troop)return nullptr;
	return troop->currentGlobalCO();
}

CommanderPowerFeature Campaign::getCommanderPowerFeature(const decltype(CommanderPower::allFeatures) &allFeatures,const Corp &corp,const TerrainCode &terrainCode,const Weather &weather,function<void(const CommanderPowerFeature&)> eachFeature)const{
	CommanderPowerFeature ret;
	//数据传递给lua环境
	luaState->doString("corp="+corp.toLuaString());//兵种
	luaState->doString("terrain="+terrainCode.toLuaString());//地形
	luaState->doString("weather="+weather.toLuaString());//天气
	allFeatures.forEach([&](const CommanderPowerFeature &cpf){//从特性中找出加成数据
		bool corpOK=true,terrainOK=true,weatherOK=true;
		if(!cpf.corpType.empty()){//查询兵种条件
			if(luaState->getGlobalFunction(cpf.corpType) && luaState->protectCall()){
				corpOK=luaState->getTopBoolean();
			}
		}
		if(!cpf.terrainType.empty()){//查询地形条件
			if(luaState->getGlobalFunction(cpf.terrainType) && luaState->protectCall()){
				terrainOK=luaState->getTopBoolean();
			}
		}
		if(!cpf.weatherType.empty()){//查询天气条件
			if(luaState->getGlobalFunction(cpf.weatherType) && luaState->protectCall()){
				weatherOK=luaState->getTopBoolean();
			}
		}
		//根据条件,累加各种数据
		if(corpOK && terrainOK && weatherOK){
			if(eachFeature)eachFeature(cpf);//单独处理
			ret+=cpf;//累加
		}
	});
	return ret;
}
CommanderPowerFeature Campaign::getCommanderPowerFeature(const UnitData &unitData,function<void(const CommanderPowerFeature&)> eachFeature)const{
	CommanderPowerFeature ret;
	auto campaignCO=getCampaignCO(*unitData.unit);
	if(campaignCO){
		auto commander=commandersList->data(campaignCO->coID);
		if(commander){//获取指挥官当前的能力发动状态
			auto power=commander->allPowers.data(campaignCO->powerLevel);
			if(power){
				auto weather=weathersList->data(currentWeatherIndex);
				if(weather){
					ret=getCommanderPowerFeature(power->allFeatures,*unitData.corp,*unitData.terrainCode,*weather,eachFeature);
				}
			}
		}
	}
	return ret;
}
decltype(Commander::allPowers)* Campaign::currentCOpowersList()const{
	//获取指挥官
	auto troop=currentTroop();
	if(troop){
		auto co=troop->currentGlobalCO();
		if(co)return co->getCOpowersList(*this);
	}
	return nullptr;
}
bool Campaign::changeCO(){return false;}
bool Campaign::changeCOpowerLevel(SizeType level){
	auto co=getCurrentGlobalCampaignCO();
	if(co){
		co->changeCoPower(*this,level);
	}
	return co;
}

bool Campaign::isSelfUnit(const CampaignTroop &campTroop,const Unit &unit)const{return campTroop.troopID==unit.color;}
bool Campaign::isFriendUnit(const CampaignTroop &campTroop,const Unit &unit)const{
	auto troop=findCampaignTroop(unit);
	return troop ? campTroop.teamID==troop->teamID : false;
}
bool Campaign::isEnemyUnit(const CampaignTroop &campTroop, const Unit &unit)const{
	return !isFriendUnit(campTroop,unit);
}
bool Campaign::isSelfTerrain(const CampaignTroop &campTroop, const Terrain &terrain)const{
	auto troop=findCampaignTroop(terrain);
	return troop ? campTroop.troopID==troop->troopID : false;
}
bool Campaign::isFriendTerrain(const CampaignTroop &campTroop, const Terrain &terrain)const{
	auto troop=findCampaignTroop(terrain);
	return troop ? campTroop.teamID==troop->teamID : false;
}
bool Campaign::isEnemyTerrain(const CampaignTroop &campTroop, const Terrain &terrain)const{
	auto troop=findCampaignTroop(terrain);
	return troop ? campTroop.teamID!=troop->teamID : false;
}

void Campaign::makeCommandString(){
	//单位的坐标
	if(selectedUnitData.unit){
		auto p=&selectedUnitData.unit->coordinate;
		commandString += Number::toString(p->x)+","+Number::toString(p->y);
	}
	//移动方向
	MovePoint last,current;
	commandString+=",";
	for(SizeType i=0;i<movePath.size();++i){
		last=current;
		current=*movePath.data(i);
		if(i==0)continue;
		//计算方向序列
		auto p=current-last;
		if(p.x==-1&&p.y==0){commandString+="L";}
		if(p.x==1&&p.y==0){commandString+="R";}
		if(p.x==0&&p.y==-1){commandString+="D";}
		if(p.x==0&&p.y==1){commandString+="U";}
	}
	//指令名称
	switch(corpMenuCommand){
#define CASE(name) case Menu_##name:commandString+=","#name;break;
		CAMPAIGN_CORPMENU(CASE)
#undef CASE
		default:;
	}
	//指令参数
}
bool Campaign::executeCommandString(const string &cmdString){
	PRINT_CYAN("执行命令:%s\n",cmdString.data());
	clearAllOperation();
	//读取坐标,获取单位
	CoordType p;
	sscanf(cmdString.data(),"%hd,%hd",&p.x,&p.y);
	auto unit=battleField->getUnit(p);
	if(!unit)return false;
	//读取移动方向,获取路径
	return true;
}

bool Campaign::moveWithPath(){
	//移动过程中,命令菜单,移动攻击范围消失
	hideCorpMenu();
	movablePoints.clear();
	firablePoints.clear();
	if(!selectedUnitData.unit)return true;
	//开始移动过程
	auto oldCoord=selectedUnitData.unit->coordinate;
	do{
		auto moved=moveOneStepWithPath();
		if(whenExecuteMoveUnit){//发生了位移,要播放动画
			whenExecuteMoveUnit(oldCoord,*selectedUnitData.unit);
			if(moved)return false;//要等动画播放完,必须返回false
		}
		if(!moved)break;
	}while(true);
	selectedUnitData.unit->isWait=true;//不管有没有碰撞,都变成待机状态
	//改变坐标
	auto srcTerrain=battleField->getTerrain(movePath_SrcCoord());
	auto dstTerrain=battleField->getTerrain(movePath_DstCoord());
	if(srcTerrain && dstTerrain && srcTerrain!=dstTerrain && srcTerrain->unitIndex!=TERRAIN_NO_UNIT){//位置发生了变化
		dstTerrain->unitIndex = srcTerrain->unitIndex;
		srcTerrain->unitIndex = TERRAIN_NO_UNIT;
	}
	return true;
}
bool Campaign::moveOneStepWithPath(){
	//确定当前位置在路径中的索引
	auto idx=movePath.indexOf([&](const MovePoint &mp){
		return mp==selectedUnitData.unit->coordinate;
	});
	if(idx<0)return false;
	//往下一个格子移动
	auto path=movePath.data(idx+1);
	if(path){
		//碰撞检测，检测是否发生碰撞
		auto unit=battleField->getUnit(*path);
		if(unit && !isFriendUnit(*selectedUnitData.campaignTroop,*unit)){//遇到了非自军友军的单位,拦截
			return false;
		}
		//无碰撞,执行移动过程
		selectedUnitData.unit->coordinate=*path;
		selectedUnitData.unit->fuel=path->remainFuel;
		return true;
	}
	return false;
}

void Campaign::showCorpMenu(){
	hideCorpMenu();
	//添加各个菜单项
	if(!showMenuItem_Join() && !showMenuItem_Load() && showMenuItem_Wait()){//要么Join要么Load,否则执行其它指令
		showMenuItem_Drop();
		showMenuItem_Fire();
		showMenuItem_Capture();
		showMenuItem_Supply();
		showMenuItem_Build();
		showMenuItem_Flare();
		showMenuItem_Hide();
		showMenuItem_Appear();
		showMenuItem_Launch();
		showMenuItem_Repair();
		showMenuItem_Explode();
	}
	corpMenu.sort([](const EnumCorpMenu &a,const EnumCorpMenu &b){return a<b;});
}
void Campaign::hideCorpMenu(){corpMenu.clear();}
bool Campaign::showMenuItem_Join(){
	if(cursorUnitData.unit){
		if(selectedUnitData.corp != cursorUnitData.corp)return false;//兵种相同才能结合
		if(selectedUnitData.campaignTroop != cursorUnitData.campaignTroop)return false;//同一支部队的才能结合
		if(selectedUnitData.unit==cursorUnitData.unit)return false;//不能自己与自己结合
		if(cursorUnitData.unit->presentHP() >= 10)return false;//被结合方的表现hp要小于10
		corpMenu.push_back(Menu_Join);
		return true;
	}
	return false;
}
bool Campaign::showMenuItem_Load(){
	if(cursorUnitData.unit){
		//确定是不是自己人
		auto troop=currentTroop();
		bool canLoad=campaignRule.allowLoadUnitOnFriendUnit?
			isFriendUnit(*troop,*cursorUnitData.unit):
			isSelfUnit(*troop,*cursorUnitData.unit);
		if(!canLoad)return false;
		//不能自己与自己结合
		if(selectedUnitData.unit==cursorUnitData.unit)return false;
		//确定载体的容积
		auto &loader=cursorUnitData;
		auto &beLoader=selectedUnitData;
		auto volumn = luaFunc_volumnOfLoader(loader.corp->name);
		if(volumn<0)return false;//不是载体
		auto weight = luaFunc_unitWeight(loader.corp->name,beLoader.corp->name,beLoader.corp->corpType);
		if(weight<0)return false;//不可被装载
		//统计已经使用的容积
		decltype(volumn) usedVolumn=0;
		for(auto &unit:loader.unit->loadedUnits){
			auto corp=getCorp(unit);
			if(corp){
				usedVolumn += luaFunc_unitWeight(loader.corp->name,corp->name,corp->corpType);
			}
		}
		if(usedVolumn + weight <= volumn){
			corpMenu.push_back(Menu_Load);
			return true;
		}
	}
	return false;
}
bool Campaign::showMenuItem_Drop(){
	//首先,查看自己有没有部队可以卸载
	for(auto &unit:selectedUnitData.unit->loadedUnits){
		if(canDropFrom(unit,cursor)){
			corpMenu.push_back(Menu_Drop);
			return true;
		}
	}
	return false;
}

bool Campaign::showMenuItem_Fire(){
	auto wpn=selectedUnitData.corp->firstAttackableWeapon();//开火条件:具有可攻击性武器
	if(!wpn)return false;
	//寻找目标
	Array<Unit*> targetUnits;
	UnitData ud(this);
	auto tryAttack=[&](const CoordType &p){
		auto unit=battleField->getUnit(p);
		if(!unit || !unit->isVisible)return;//过滤不可见单位
		if(isFriendUnit(*selectedUnitData.campaignTroop,*unit))return;//过滤掉自军和友军单位
		//判断攻击类型
		ud.getUnitData(*unit);
		if(damageCaculator->canAttack(selectedUnitData,ud,cursor)){
			targetUnits.push_back(unit);
		}
	};
	auto destP=movePath_DstCoord();
	if(wpn->isDirectAttack()){//近程攻击,只判断周围
		caculateRange(destP,1,tryAttack);
	}
	if(wpn->isIndirectAttack() && selectedUnitData.unit->coordinate==destP){//远程攻击,判断射程范围内
		caculateRange(destP,wpn->minRange,wpn->maxRange,tryAttack);
	}
	//更新目标点
	targetPoints.clear();
	for(auto &pUnit:targetUnits){
		targetPoints.push_back(pUnit->coordinate);
	}
	//生成开火指令
	if(targetPoints.size()){
		corpMenu.push_back(Menu_Fire);
		return true;
	}
	return false;
}
bool Campaign::showMenuItem_Capture(){
	if(!selectedUnitData.corp->capturable)return false;//兵种要具有占领能力
	if(!cursorUnitData.terrainCode->capturable)return false;//目标地形必须是可占领的据点
	if(isFriendTerrain(*selectedUnitData.campaignTroop,*cursorUnitData.terrain))return false;//自己不能占领友军的据点
	//可占领
	corpMenu.push_back(Menu_Capture);
	return true;
}
bool Campaign::showMenuItem_Supply(){
	if(!selectedUnitData.corp->suppliable)return false;//兵种必须有补给能力
	caculateSuppliableUnits(false);
	if(suppliableUnits.size()){//有可补给的单位就显示此指令
		corpMenu.push_back(Menu_Supply);
	}
	return suppliableUnits.size();
}
bool Campaign::showMenuItem_Build(){
	if(selectedUnitData.corp->buildable){
		auto toBuild=luaFunc_buildTerrain(cursorUnitData.terrainCode->name);
		if(!toBuild.empty()){
			corpMenu.push_back(Menu_Build);
			return true;
		}
	}
	return false;
}
bool Campaign::showMenuItem_Flare(){
	auto wpn=selectedUnitData.corp->firstFlarableWeapon();
	if(wpn && selectedUnitData.unit->ammunition){//带有闪光功能,且有弹药
		if(selectedUnitData.unit->coordinate==cursor){//没有移动
			corpMenu.push_back(Menu_Flare);
			return true;
		}
	}
	return false;
}
bool Campaign::showMenuItem_Hide(){
	if(selectedUnitData.corp->hidable && !selectedUnitData.unit->isHide){
		corpMenu.push_back(Menu_Hide);
		return true;
	}
	return false;
}
bool Campaign::showMenuItem_Appear(){
	if(selectedUnitData.corp->hidable && selectedUnitData.unit->isHide){
		corpMenu.push_back(Menu_Appear);
		return true;
	}
	return false;
}
bool Campaign::showMenuItem_Launch(){
	if(selectedUnitData.corp->capturable && !cursorUnitData.terrainCode->terrainAfterLanuch.empty()){//可执行发射指令
		corpMenu.push_back(Menu_Launch);
		return true;
	}
	return false;
}
bool Campaign::showMenuItem_Repair(){
	if(!selectedUnitData.corp->repairable)return false;
	caculateSuppliableUnits(true);
	if(suppliableUnits.size()){//有可补给的单位就显示此指令
		corpMenu.push_back(Menu_Repair);
	}
	return suppliableUnits.size();
}
bool Campaign::showMenuItem_Explode(){
	if(selectedUnitData.corp->explodable){
		corpMenu.push_back(Menu_Explode);
		return true;
	}
	return false;
}
bool Campaign::showMenuItem_Wait(){
	auto trn=battleField->getTerrain(movePath_DstCoord());
	if(trn){
		auto unit=battleField->getUnit(*trn);
		if(!unit || unit==selectedUnitData.unit){//移动到无单位的位置,或者原地待机时候可显示
			corpMenu.push_back(Menu_Wait);
		}
	}
	return corpMenu.contain(Menu_Wait);
}

//执行命令
#define MOVE_WITH_PATH if(!moveWithPath())return false;

bool Campaign::execMenuItem_Join(){
	MOVE_WITH_PATH
	//开始结合
	auto unitA=selectedUnitData.unit,unitB=cursorUnitData.unit;
	auto corp=selectedUnitData.corp;
	unitB->fuel += unitA->fuel;//累加燃料
	unitB->fuel = min(unitB->fuel,corp->gasMax);//燃料不超过上限
	unitB->ammunition += unitA->ammunition;//累加弹药
	auto weapon = corp->firstAttackableWeapon();
	if(weapon){
		unitB->ammunition = min(unitB->ammunition,weapon->ammunitionMax);//弹药不能超过上限
	}
	//HP
	auto presentHP = unitA->presentHP() + unitB->presentHP();
	auto factHP = unitA->healthPower + unitB->healthPower;
	if(presentHP>10){//处理表现HP超出10的情况
		auto pastHP=presentHP-10;//多出来的表现HP
		presentHP=10;
		selectedUnitData.campaignTroop->funds += corp->price * pastHP /10;//我们应该转换成价值
	}
	//调整一下精确的hp
	decltype(factHP) minimum = (presentHP-1)*10+1,maximum = presentHP*10;
	factHP=max(minimum,factHP);//不低于下限
	factHP=min(factHP,maximum);//不高于上限
	unitB->healthPower = factHP;
	//要删除掉A,以B的状态为准
	battleField->removeUnit(*unitA);
	return true;
}
bool Campaign::execMenuItem_Load(){
	MOVE_WITH_PATH
	cursorUnitData.unit->loadedUnits.push_back(*selectedUnitData.unit);
	battleField->removeUnit(*selectedUnitData.unit);
	return true;
}
bool Campaign::execMenuItem_Drop(){
	if(unitToDrop && selectedTargetPoint){//执行卸载动作
		MOVE_WITH_PATH
		UnitData userData(this);
		userData.getUnitData(*selectedTargetPoint);
		if(userData.unit){//有障碍!
			corpMenuCommand = AmountOfCorpEnumMenu;//命令停止执行
		}else{
			//改变自身状态,然后从载体上下来
			unitToDrop->coordinate.setP(*selectedTargetPoint);//下载体后的坐标
			unitToDrop->isWait=true;//待机状态
			battleField->addUnit(*unitToDrop);//跑到地图上了
			selectedUnitData.unit->loadedUnits.remove(unitToDrop);//从搭载列表中移除
			//继续执行卸载命令
			cursor = selectedUnitData.unit->coordinate;//备份一下坐标
		}
		return true;
	}else{//要显示菜单
		unitsArray.clear();
		for(auto &unit:selectedUnitData.unit->loadedUnits){
			if(canDropFrom(unit,cursor)){
				unitsArray.push_back(&unit);
			}
		}
		return false;
	}
}
//菜单指令执行
bool Campaign::execMenuItem_Fire(){
	if(selectedTargetPoint){//如果选择了目标,则开火
		MOVE_WITH_PATH//先移动
		damageCaculator->executeAttack();//后开火
		UnitData* ud[]={&selectedUnitData,&cursorUnitData};
		for(UnitData* &u:ud){//移除掉被干掉的部队
			if(u->unit->healthPower<=0)battleField->removeUnit(*u->unit);
		}
		return true;
	}else{//进入选择目标的状态
		hideCorpMenu();//菜单消失
		selectedTargetPoint=targetPoints.firstData();
		selectedTargetPointFreely=false;
		chooseTarget(0);
	}
	return false;
}
bool Campaign::execMenuItem_Capture(){
	MOVE_WITH_PATH
	auto unit=selectedUnitData.unit;
	//计算占领速度
	auto feature=getCommanderPowerFeature(selectedUnitData);
	//占领量=10*(表现HP/10)*(100%+速度百分比),向下取整数
	unit->progressValue += Number::divideFloor(10*unit->presentHP()*(100+feature.captureSpeed),1000);
	if(unit->progressValue>=campaignRule.captureProgressMax){//占领完成的时候就改变状态
		unit->progressValue=0;
		cursorUnitData.terrain->status = unit->color;
	}
	return true;
}
bool Campaign::execMenuItem_Supply(){
	for(auto &pUnit:suppliableUnits){
		supplyUnit(*pUnit);
	}
	return true;
}
bool Campaign::execMenuItem_Build(){
	MOVE_WITH_PATH
	auto unit=selectedUnitData.unit;
	unit->progressValue += unit->presentHP();//自己增长自己的进度值(占领值)
	if(unit->progressValue>=campaignRule.buildProgressMax){//占领完成的时候就改变状态
		unit->progressValue=0;
		auto toBuild=luaFunc_buildTerrain(selectedUnitData.terrainCode->name);
		if(!toBuild.empty()){
			SizeType pos;
			auto code=battleField->terrainsList->dataName(toBuild,pos);
			if(pos>=0 && code->capturable){//建造成功
				cursorUnitData.terrain->terrainType = pos;
				cursorUnitData.terrain->status = unit->color;
			}
		}
	}
	return true;
}
bool Campaign::execMenuItem_Flare(){
	if(selectedTargetPoint){
		if(!targetPoints.contain(cursor)){//取消操作
			firablePoints.clear();
			targetPoints.clear();
			showCorpMenu();
		}
		//发射闪光弹
		--selectedUnitData.unit->ammunition;//弹药减少
		auto wpn=selectedUnitData.corp->firstFlarableWeapon();
		//生成闪光点
		FlarePoint fp;
		fp.setP(selectedTargetPointFreely ? cursor : *selectedTargetPoint);//自由选择时候以光标为准
		if(wpn)fp.flareRange=wpn->flareRange;
		fp.troopID=selectedUnitData.campaignTroop->troopID;
		flarePoints.push_back(fp);
		return true;
	}else{//选择发射闪光弹的位置
		hideCorpMenu();
		movablePoints.clear();
		firablePoints.clear();
		targetPoints.clear();
		auto wpn=selectedUnitData.corp->firstFlarableWeapon();//获取武器数据,取其射程
		if(wpn){
			caculateRange(selectedUnitData.unit->coordinate,wpn->minRange,wpn->maxRange,[&](const CoordType &p){
				if(battleField->isInRange(p.x,p.y)){
					firablePoints.push_back(p);
					targetPoints.push_back(p);
				}
			});
		}
		//自由选择目标
		selectedTargetPoint=targetPoints.data(0);
		selectedTargetPointFreely=true;
	}
	return false;
}
bool Campaign::execMenuItem_Hide(){
	MOVE_WITH_PATH
	selectedUnitData.unit->isHide=true;
	return true;
}
bool Campaign::execMenuItem_Appear(){
	MOVE_WITH_PATH
	selectedUnitData.unit->isHide=false;
	return true;
}
bool Campaign::execMenuItem_Launch(){
	if(selectedTargetPoint){//发射火箭
		MOVE_WITH_PATH
		caculateRange(*selectedTargetPoint,0,campaignRule.launchImpactRange,[&](const CoordType &p){//范围内的单位都受伤
			auto unit=battleField->getUnit(p);
			if(unit){
				unit->healthPower = unit->healthPower>campaignRule.launchImpactDamage ? unit->healthPower-campaignRule.launchImpactDamage : 1;//削弱30%的HP
			}
		});
		//地形改变
		auto &p=selectedUnitData.unit->coordinate;
		auto terrain=battleField->getTerrain(p);
		if(terrain){
			auto trnCode=getTerrainCode(*terrain);
			if(trnCode){
				battleField->setTerrain(p.x,p.y,trnCode->terrainAfterLanuch);
			}
		}
		return true;
	}else{//选择发射地点
		hideCorpMenu();
		selectedTargetPoint=&cursor;
		selectedTargetPointFreely=true;
	}
	return false;
}
bool Campaign::execMenuItem_Repair(){
	if(selectedTargetPoint){//执行修复
		MOVE_WITH_PATH
		supplyUnit(*cursorUnitData.unit);//补给
		repairUnit(*cursorUnitData.unit,campaignRule.unitRepairHP);
		return true;
	}else{//选择修复目标
		hideCorpMenu();
		for(auto &pUnit:suppliableUnits){
			targetPoints.push_back(pUnit->coordinate);
		}
		selectedTargetPoint=targetPoints.firstData();
		chooseTarget(0);
	}
	return false;
}
bool Campaign::execMenuItem_Explode(){
	firablePoints.clear();
	if(selectedTargetPoint){//开始爆炸
		if(!targetPoints.contain(cursor)){//取消爆炸
			showCorpMenu();
			return false;
		}
		MOVE_WITH_PATH
		//爆炸,波及范围内所有单位会受伤
		caculateRange(movePath_DstCoord(),0,campaignRule.unitExplodeRange,[&](const CoordType &p){
			auto unit=battleField->getUnit(p);
			if(unit)reduceUnitHP(*unit,campaignRule.unitExplodeDamage);
		});
	}else{//显示波及范围
		hideCorpMenu();
		caculateRange(cursor,0,2,[&](const CoordType &p){
			firablePoints.push_back(p);
		});
		//固定目标点
		targetPoints.clear();
		targetPoints.push_back(cursor);
		selectedTargetPoint=targetPoints.firstData();
		chooseTarget(0);
	}
	return false;
}
bool Campaign::execMenuItem_Wait(){return moveWithPath();}

//存档/读档
bool Campaign::saveCampaign(const string &filename)const{
	auto file=fopen(filename.data(),"wb");
	ASSERT_RETURN(file,ErrorNumber::getErrorString(errno))
	//保存地图名
	fprintf(file,"%s",battleField->mapFilename.data());
	//保存地形变化情况
	battleField->saveMap_CSV(file);
	//保存单位表
	//保存参战的势力和CO
	//保存天气情况
	//保存具体规则
	//完毕
	fflush(file);fclose(file);
	return true;
}
bool Campaign::loadCampaign(const string &filename){return false;}
