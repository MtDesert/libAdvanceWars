#include"Campaign.h"
#include"DamageCaculator.h"

CampaignCO::CampaignCO():coID(0),energy(0),powerStatus(0){}
CampaignTroop::CampaignTroop():troopID(0),isAI(false),teamID(0),funds(0),isLose(false){}
CampaignRule::CampaignRule():mIsFogWar(false),mInitFunds(0),mBaseIncome(1000),
mBasesToWin(0),mFundsToWin(0),mTurnsToWin(0),mUnitLevel(0),mCoPowerLevel(2){}
UnitData::UnitData(Campaign *cmpgn):campaign(cmpgn){clear();}

void UnitData::getUnitData(const decltype(Unit::coordinate) &p){
	if(!campaign)return;
	//获取地形信息
	auto field = campaign->battleField;
	terrain = field->getTerrain(p.x,p.y);
	terrainCode = terrain ? field->terrainsList->data(terrain->terrainType) : nullptr;
	//获取单位信息
	unit = campaign->battleField->chessPieces.data([&](const Unit &unit){return unit.coordinate==p;});
	corp = unit ? field->corpsList->data(unit->corpType) : nullptr;
	//获取所属部队信息
	campaignTroop = unit ? campaign->allTroops.data([&](const CampaignTroop &troop){return troop.troopID==unit->color;}) : nullptr;
}
void UnitData::clear(){
	unit=nullptr;
	corp=nullptr;
	terrain=nullptr;
	terrainCode=nullptr;
	campaignTroop=nullptr;
}

Campaign::Campaign():battleField(nullptr),damageCaculator(nullptr),
currentTroopIndex(0),currentWeatherIndex(0),selectedTargetPoint(nullptr),corpMenuCommand(AmountOfCorpEnumMenu),
currentWeather(nullptr),unitToDrop(nullptr),whenError(nullptr){
	cursorUnitData.campaign=this;
	selectedUnitData.campaign=this;
}
Campaign::~Campaign(){}

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
			troop->troopID=i;
			troop->teamID=teamID;
		}
	}
}
void Campaign::endTurn(){
	//清除待机状态(只清除当前部队的)
	auto troop=allTroops.data(currentTroopIndex);
	if(troop){
		for(auto &unit:battleField->chessPieces){
			if(unit.color==troop->troopID){
				unit.isWait=false;
			}
		}
	}
	//下一个队伍行动
	nextCampaignTroopTurn();
	printf("troop[%lu] turn\n",currentTroopIndex);
}
void Campaign::nextCampaignTroopTurn(){
	auto oldIndex=currentTroopIndex;
	do{
		//改变当前部队
		++currentTroopIndex;
		auto sz=allTroops.size();
		currentTroopIndex = sz ? currentTroopIndex % sz : 0;
		//跳过已经输掉的部队
		auto troop=allTroops.data(currentTroopIndex);
		if(troop && !troop->isLose)break;//到没输的部队行动
	}while(currentTroopIndex!=oldIndex);//防止无限循环
}

void Campaign::setCursor(const CoordType &p){
	if(!battleField->isInRange(p.x,p.y))return;//过滤
	if(selectedTargetPoint){//需要选择目标,这里要判断
		auto tp=targetPoints.data([&](const CoordType &tp){return tp==p;});
		if(tp){
			cursor=p;
			cursorUnitData.getUnitData(p);
		}
	}else{
		cursor=p;
		cursorUnitData.getUnitData(p);
		if(movablePoints.size()){
			selectPath(p);
		}
	}
}

static const Campaign::CoordType pUp(0,1),pDown(0,-1),pLeft(-1,0),pRight(1,0);
static const Campaign::CoordType directionP[]={{0,1},{0,-1},{-1,0},{1,0}};
//LUA交互
int Campaign::luaFunc_movementCost(const string &moveType,const string &terrainName,const string &weatherName){
	int ret=-1;
	if(luaState.getGlobalFunction("movementCost")){
		luaState.push(moveType).push(terrainName).push(weatherName);
		if(luaState.protectCall()){
			luaState.getTopInteger(ret);
		}
	}
	return ret;
}
int Campaign::luaFunc_fuelCost(const string &weatherName){
	int ret=-1;
	if(luaState.getGlobalFunction("fuelCost")){
		luaState.push(weatherName);
		if(luaState.protectCall()){
			luaState.getTopInteger(ret);
		}
	}
	return ret;
}
int Campaign::luaFunc_volumnOfLoader(const string &loaderName){
	int ret=-1;
	if(luaState.getGlobalFunction("volumnOfLoader")){
		luaState.push(loaderName);
		if(luaState.protectCall()){
			luaState.getTopInteger(ret);
		}
	}
	return ret;
}
int Campaign::luaFunc_unitWeight(const string &loaderName,const string &beLoaderName,const string &beLoaderCorpType){
	int ret=-1;
	if(luaState.getGlobalFunction("unitWeight")){
		luaState.push(loaderName).push(beLoaderName).push(beLoaderCorpType);
		if(luaState.protectCall()){
			luaState.getTopInteger(ret);
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
	//这里加入CO对移动力的修正,对了还有个别CO是可以以地形损耗1通过所有地形的
	//清除缓冲
	movementCostCache.setSize(battleField->terrainsList->size(),true);
	for(auto &move:movementCostCache)move=-1;
	//开始对点进行推算
	movablePoints.clear();
	movablePoints.push_back(mp);
	movePath.clear();
	movePath.push_back(mp);
	//取天气,开始计算
	currentWeather=weathesList->data(currentWeatherIndex);
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
			moveCost=luaFunc_movementCost(selectedUnitData.corp->moveType,trnCode->name,currentWeather->name);
			(*movementCostCache.data(terrain->terrainType))=moveCost;//缓存起来
			if(moveCost<0)return;
		}
	}
	//碰撞检测，检测是否发生碰撞
	for(auto &unit:battleField->chessPieces){
		if(unit.coordinate==targetPos && unit.color != selectedUnitData.unit->color && unit.isVisible){
			auto troop=findCampaignTroop(unit);//判断是敌人还是朋友,不是朋友的一般都会采取拦截措施
			if(!troop)return;
			if(troop->teamID != selectedUnitData.campaignTroop->teamID)return;
		}
	}
	//检查剩余的移动力和燃料
	//if moveCost1 then moveCost=1 end某些指挥官貌似走可走的地形都是1
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
	int moveCost=luaFunc_movementCost(selectedUnitData.corp->moveType,cursorUnitData.terrainCode->name,currentWeather->name);
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
bool Campaign::canStayAt(const string &moveType,const CoordType &p,bool checkBarrier){
	UnitData unitData(this);
	unitData.getUnitData(p);
	if(!unitData.terrainCode)return false;
	int moveCost=luaFunc_movementCost(moveType,unitData.terrainCode->name,currentWeather->name);
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
bool Campaign::canDropFrom(const Unit &unit, const CoordType &p,bool needSave){
	auto corp=battleField->corpsList->data(unit.corpType);
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

bool Campaign::moveWithPath(){
	bool ret=true;
	for(auto &path:movePath){
		//碰撞检测，检测是否发生碰撞
		for(auto &unit:battleField->chessPieces){
			if(unit.coordinate==path && unit.color != selectedUnitData.unit->color){
				//判断是敌人还是朋友,不是朋友的一般都会采取拦截措施
				auto troop=findCampaignTroop(unit);
				if(!troop)ret=false;
				if(troop->teamID != selectedUnitData.campaignTroop->teamID)ret=false;
				if(!ret)break;
			}
		}
		if(!ret)break;
		//无碰撞,执行移动过程
		selectedUnitData.unit->coordinate=path;
		selectedUnitData.unit->fuel=path.remainFuel;
	}
	selectedUnitData.unit->isWait=true;//不管有没有碰撞,都变成待机状态
	return ret;
}

void Campaign::caculateFlightshot_byMovement(){//根据移动范围来计算射程
	firablePoints.push_back(selectedUnitData.unit->coordinate);
	for(SizeType i=0;i<movablePoints.size();++i){
		auto p=*movablePoints.data(i);
		for(auto &dir:directionP)firablePoints.push_back(p+dir);
	}
	firablePoints.unique();//去掉重复
	caculateFlightshot_removeNotInRange();//移除超出地图的点
}
void Campaign::caculateFlightshot_byCenter(){
	firablePoints.clear();
	if(selectedUnitData.unit && selectedUnitData.corp){
		auto wpn=selectedUnitData.corp->weapons.data(0);//取首个武器判断
		if(wpn && wpn->isIndirectAttack()){
			caculateFlightshot_byCenter(selectedUnitData.unit->coordinate,wpn->minRange,wpn->maxRange);
		}
	}
}
void Campaign::caculateFlightshot_byCenter(const CoordType &center,int minDistance,int maxDistance){
	for(int i=minDistance;i<=maxDistance;++i){//计算射程
		caculateFlightshot_byCenter(center,i);
	}
	caculateFlightshot_removeNotInRange();
}
void Campaign::caculateFlightshot_byCenter(const CoordType &center,int distance){
	int d=distance;
	for(int i=1;i<=d;++i){
		firablePoints.push_back(CoordType(center.x+i,center.y+(i-d)));
		firablePoints.push_back(CoordType(center.x+(d-i),center.y+i));
		firablePoints.push_back(CoordType(center.x-i,center.y+(d-i)));
		firablePoints.push_back(CoordType(center.x+(i-d),center.y-i));
	}
}
void Campaign::caculateFlightshot_removeNotInRange(){
	firablePoints.remove_if([this](const CoordType &p){
		return !battleField->isInRange(p.x,p.y);
	});
}

void Campaign::cursorConfirm(){
	if(selectedUnitData.unit){
		if(selectedTargetPoint){//选定了目标,再次执行指令
			bool executeFinish=false;
			switch(corpMenuCommand){
#define CASE(name) case Menu_##name:executeFinish=execMenuItem_##name();break;
				CASE(Fire)
				CASE(Drop)
#undef CASE
				default:printf("execute %d ???\n",corpMenuCommand);
			}
			//清除操作结果
			if(executeFinish)clearAllOperation();
		}else{//没有选定目标,尝试生成兵种命令菜单
			auto lastP=movePath.lastData();
			if(lastP && *lastP==cursor){//在移动范围内
				auto troop=allTroops.data(currentTroopIndex);
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
		if(cursorUnitData.unit && cursorUnitData.corp){//如果光标处有单位,则计算移动范围
			selectedUnitData=cursorUnitData;
			caculateMovement(*selectedUnitData.unit,*selectedUnitData.corp);//计算移动范围
			if(selectedUnitData.corp->isDirectAttack()){//计算火力范围(直接攻击部队的情况下)
				caculateFlightshot_byMovement();
				for(auto &p:movablePoints){//移除和移动范围一样的点
					firablePoints.remove(p);
				}
			}
		}//如果没有单位,则有可能是制造单位的工厂
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
	corpMenu.clear();
	produceMenu.clear();
	//清除刚操作的单位信息
	selectedUnitData.clear();
	cursorUnitData.getUnitData(cursor);
	//清除各种缓冲
	selectedTargetPoint=nullptr;
	unitsArray.clear();
	unitToDrop=nullptr;
}

void Campaign::executeCorpMenu(int index){
	//根据idx确定要执行的command
	auto command=corpMenu.data(index);
	corpMenuCommand = command ? *command : AmountOfCorpEnumMenu;
	if(!command)return;
	//执行
	bool executeFinish=false;
	switch(*command){
#define CASE(name) case Menu_##name:executeFinish=execMenuItem_##name();break;
		CAMPAIGN_CORPMENU(CASE)
#undef CASE
		default:;
	}
	//执行完毕就清除相关操作
	if(executeFinish){
		clearAllOperation();
	}
}
void Campaign::executeProduceMenu(int index){
	//根据idx确定要执行的command
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
	auto p=movePath.lastData();//如果unit是首个卸载的单位,movePath应该是有数据的
	canDropFrom(unit,(p ? *p : selectedUnitData.unit->coordinate),true);
	if(targetPoints.size()>0){
		unitToDrop=&unit;
		selectedTargetPoint=targetPoints.firstData();
		chooseTarget(0);
		corpMenu.clear();//兵种命令菜单消失
		unitsArray.clear();//单位菜单消失
		return true;
	}
	return false;
}

void Campaign::showCorpMenu(){
	corpMenu.clear();
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
			auto corp=battleField->corpsList->data(unit.corpType);
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
	auto destP=movePath.lastData();
	if(!destP)return false;
	//近程攻击,需要判断目标点附近有没有敌人
	Array<Unit*> targetUnits;
	auto selectCorp=selectedUnitData.corp;
	if(selectCorp->isDirectAttack()){
		battleField->getUnits(*destP,1,1,targetUnits);
	}
	//远程攻击,需要判断射程
	if(selectCorp->isIndirectAttack()){
		auto wpn=selectCorp->weapons.data(0);//首个武器
		battleField->getUnits(*destP,wpn->minRange,wpn->maxRange,targetUnits);
	}
	//过滤掉自军和友军单位
	targetUnits.remove_if([&](Unit* const &unit){
		auto troop=findCampaignTroop(*unit);
		if(!troop)return false;
		return troop->troopID==selectedUnitData.campaignTroop->troopID;
	});
	//检查自身的武器能否攻击对方
	targetUnits.remove_if([&](Unit* const &unit){
		auto corp=battleField->corpsList->data(unit->corpType);
		if(!corp)return false;
		int dmg=-1;
		for(int i=0;i<2;++i){//扫描主副武器
			dmg = damageCaculator->corpDamage(*selectCorp,*corp,i);
			if(dmg>=0)break;
		}
		return dmg<0;
	});
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
	if(selectedUnitData.unit->color == cursorUnitData.terrain->status)return false;//自己不能占领自己
	//自己不能占领友军的据点
	auto troop=findCampaignTroop(*cursorUnitData.terrain);
	if(troop && troop->teamID==selectedUnitData.campaignTroop->teamID)return false;
	//可占领
	corpMenu.push_back(Menu_Capture);
	return true;
}
bool Campaign::showMenuItem_Supply(){
	suppliableUnits.clear();
	//首先必须位置相邻,且不是自己
	//必须是自军或者友军
	if(suppliableUnits.size()){//有可补给的单位就显示此指令
		corpMenu.push_back(Menu_Supply);
	}
	return suppliableUnits.size();
}
bool Campaign::showMenuItem_Build(){
	return false;
}
bool Campaign::showMenuItem_Flare(){
	return true;
}
bool Campaign::showMenuItem_Hide(){
	return true;
}
bool Campaign::showMenuItem_Appear(){
	return true;
}
bool Campaign::showMenuItem_Launch(){
	//主要是占领系的兵种可以发射火箭
	return false;
}
bool Campaign::showMenuItem_Repair(){
	suppliableUnits.clear();
	//首先必须位置相邻,且不是自己
	//必须是自军或者友军
	if(suppliableUnits.size()){//有可补给的单位就显示此指令
		corpMenu.push_back(Menu_Repair);
	}
	return suppliableUnits.size();
}
bool Campaign::showMenuItem_Explode(){
	return true;
}
bool Campaign::showMenuItem_Wait(){
	auto destP=movePath.lastData();
	Array<Unit*> unitArr;
	battleField->getUnits(*destP,unitArr);
	//移动到无单位的位置,或者原地待机时候可显示
	if(unitArr.size()<=0 || *unitArr.firstData()==selectedUnitData.unit){
		corpMenu.push_back(Menu_Wait);
		return true;
	}
	return false;
}

//执行命令
#define MOVE_WITH_PATH if(!moveWithPath())return true;
bool Campaign::execMenuItem_Join(){
	MOVE_WITH_PATH
	//开始结合
	auto unitA=selectedUnitData.unit,unitB=cursorUnitData.unit;
	auto corp=selectedUnitData.corp;
	unitB->fuel += unitA->fuel;//累加燃料
	unitB->fuel = min(unitB->fuel,corp->gasMax);//燃料不超过上限
	unitB->ammunition += unitA->ammunition;//累加弹药
	auto weapon=corp->weapons.firstData();
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
			printf("!!!!\n");
		}else{
			//改变自身状态,然后从载体上下来
			unitToDrop->coordinate.setP(*selectedTargetPoint);//下载体后的坐标
			unitToDrop->isWait=true;//待机状态
			battleField->addUnit(*unitToDrop);//跑到地图上了
			selectedUnitData.unit->loadedUnits.remove(unitToDrop);//从搭载列表中移除
			//继续执行卸载命令
			cursor = selectedUnitData.unit->coordinate;//备份一下坐标
			clearAllOperation();
			selectedUnitData.getUnitData(cursor);//这时候载体已经开过去了
			execMenuItem_Drop();
			//检查看看还有没有能卸载的部队
			if(unitsArray.size()>0)return false;
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
		return true;
	}else{//进入选择目标的状态
		selectedTargetPoint=targetPoints.firstData();
		setCursor(*selectedTargetPoint);
	}
	return false;
}
bool Campaign::execMenuItem_Capture(){
	MOVE_WITH_PATH
	auto unit=selectedUnitData.unit;
	unit->progressValue += unit->presentHP();//自己增长自己的进度值(占领值)
	if(unit->progressValue>=20){//占领完成的时候就改变状态
		unit->progressValue=0;
		cursorUnitData.terrain->status = unit->color;
	}
	unit->isWait=true;
	return true;
}
bool Campaign::execMenuItem_Supply(){return false;}
bool Campaign::execMenuItem_Build(){return false;}
bool Campaign::execMenuItem_Flare(){return false;}
bool Campaign::execMenuItem_Hide(){return false;}
bool Campaign::execMenuItem_Appear(){return false;}
bool Campaign::execMenuItem_Launch(){return false;}
bool Campaign::execMenuItem_Repair(){return false;}
bool Campaign::execMenuItem_Explode(){return false;}
bool Campaign::execMenuItem_Wait(){
	if(moveWithPath()){
		selectedUnitData.unit->isWait=true;
	}
	return true;
}