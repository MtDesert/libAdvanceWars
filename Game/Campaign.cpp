#include"Campaign.h"
#include"Map.h"

int tolua_AdvanceWars_open(lua_State* tolua_S);

Campaign::Campaign():
	battleField(nullptr),
	currentTeam(nullptr),
	currentViewer(nullptr),
	cursorTerrainCode(nullptr),
	cursorUnit(nullptr),
	cursorCorp(nullptr),
	cursorTeam(nullptr),
	selectUnit(nullptr),
	selectCorp(nullptr),
	selectTeam(nullptr),
	isEditMode(false),
	luaState(nullptr){}
Campaign::~Campaign(){}

CampaignTeam* Campaign::findTeam(int teamID)const{
	for(auto &team:allTeams){
		if(team.teamID==teamID){
			return &team;
		}
	}
	return nullptr;
}

void Campaign::setCursor(int x,int y){
	//获取地形
	if(battleField->getTerrain(x,y,cursorTerrain)){
		cursor.x=x;cursor.y=y;//赋值
		cursorTerrainCode=battleField->terrainsList->data(cursorTerrain.terrainType);
	}else return;
	//获取单位
	cursorUnit=nullptr;
	for(auto &unit:battleField->chessPieces){
		if(unit.coordinate.x==x && unit.coordinate.y==y){
			cursorUnit=&unit;
			break;
		}
	}
	//获取兵种信息
	cursorCorp=nullptr;
	cursorTeam=nullptr;
	if(cursorUnit){
		cursorCorp=battleField->corpsList->data(cursorUnit->corpType);
		cursorTeam=findTeam(cursorUnit->color);
	}

	//判断可能的操作
	if(movablePoints.size()){
		selectPath(CoordType(x,y));
	}
}

static const Campaign::CoordType pUp(0,1),pDown(0,-1),pLeft(-1,0),pRight(1,0);

int Campaign::luaFunc_movementCost(const string &moveType,const string &terrainName,const string &weatherName)const{
	int ret=-1;
	lua_getglobal(luaState,"movementCost");
	lua_pushstring(luaState,moveType.data());//移动类型名
	lua_pushstring(luaState,terrainName.data());//地形名
	lua_pushstring(luaState,weatherName.data());//天气名
	if(lua_pcall(luaState,3,1,0)==LUA_OK){
		if(!lua_isnil(luaState,-1)){
			ret=lua_tointeger(luaState,-1);
		}
	}else{
		printf("%s\n",lua_tostring(luaState,-1));
	}
	return ret;
}
int Campaign::luaFunc_fuelCost(const string &weatherName)const{
	int ret=-1;
	lua_getglobal(luaState,"fuelCost");
	lua_pushstring(luaState,weatherName.data());//天气名
	if(lua_pcall(luaState,1,1,0)==LUA_OK){
		if(lua_isnil(luaState,-1))return ret;
		else{
			ret=lua_tointeger(luaState,-1);
		}
	}else{
		printf("%s\n",lua_tostring(luaState,-1));
	}
	return ret;
}
int Campaign::luaFunc_loadableAmount(const string &loaderCorpName,const string &beLoaderCorpName)const{
	int ret=-1;
	lua_getglobal(luaState,"loadableAmount");
	lua_pushstring(luaState,loaderCorpName.data());
	lua_pushstring(luaState,beLoaderCorpName.data());
	if(lua_pcall(luaState,2,1,0)==LUA_OK){
		if(lua_isnil(luaState,-1))return ret;
		else{
			ret=lua_tointeger(luaState,-1);
		}
	}else{
		printf("%s\n",lua_tostring(luaState,-1));
	}
	return ret;
}

static Map<int,int> movementCostCache;//<地形编号,移动损耗>缓冲
void Campaign::caculateMovement(const Unit &unit){
	if(!cursorCorp)return;
	//记录起点信息、剩余移动力、剩余燃料
	MovePoint mp;
	mp.x=unit.coordinate.x;
	mp.y=unit.coordinate.y;
	mp.remainMovement=cursorCorp->movement;
	mp.remainFuel=unit.fuel;
	//这里加入CO对移动力的修正,对了还有个别CO是可以以地形损耗1通过所有地形的
	//开始对点进行推算
	movablePoints.clear();
	movablePoints.push_back(mp);
	movePath.clear();
	movePath.push_back(mp);

	for(auto &p:movablePoints){
		tryToMoveTo(p,pUp);
		tryToMoveTo(p,pDown);
		tryToMoveTo(p,pLeft);
		tryToMoveTo(p,pRight);
	}
	movementCostCache.clear();//计算完成,清除缓冲
	//根据情况计算攻击范围
	caculateFlightshot_byMovement();
	/*if select.corp.weapons then
		local min=select.corp.weapons[1].minRange
		local max=select.corp.weapons[1].maxRange
		if min==1 and max==1 then
			caculateFlightshot_byMovement()
		else
			caculateFlightshot_byCenter(select.unit.coordinate,min,max)
		end
	end*/
}

void Campaign::tryToMoveTo(const MovePoint &currentPos,const CoordType &offset){
	CoordType targetPos=currentPos;
	targetPos.x+=offset.x;
	targetPos.y+=offset.y;
	//坐标边界判定
	Terrain terrain;
	if(!battleField->getTerrain(targetPos.x,targetPos.y,terrain))return;
	//获取地形信息，判断是否可移动的地形
	int moveCost=0;
	auto intPtr=movementCostCache.value(terrain.terrainType);//搜索缓冲
	if(intPtr){//有缓冲结果,可直接用
		if(*intPtr>=0)moveCost=*intPtr;
		else return;
	}else{//无缓冲结果,查询地形表
		auto trnCode=battleField->terrainsList->data(terrain.terrainType);
		//传递给lua来判断损耗
		moveCost=luaFunc_movementCost(cursorCorp->moveType,trnCode->name,"Normal");
		movementCostCache.insert(terrain.terrainType,moveCost);//缓存起来
		if(moveCost<0)return;
	}
	//碰撞检测，检测是否发生碰撞
	for(auto &unit:battleField->chessPieces){
		if(unit.coordinate==targetPos && unit.color!=selectUnit->color && unit.isVisible){
			auto team=findTeam(unit.color);//判断是敌人还是朋友,不是朋友的一般都会采取拦截措施
			if(!team)return;
			if(!team->friendsTeams.contain(selectUnit->color))return;
		}
	}
	//检查剩余的移动力和燃料
	//if moveCost1 then moveCost=1 end某些指挥官貌似走可走的地形都是1
	if(currentPos.remainMovement < moveCost)return;
	//传递给lua来判断损耗
	int costFuel=luaFunc_fuelCost("Normal");
	if(costFuel<0)return;
	if(currentPos.remainFuel < costFuel)return;

	//创建新的移动点
	MovePoint p;
	p.x=targetPos.x;p.y=targetPos.y;
	p.remainMovement = currentPos.remainMovement - moveCost;
	p.remainFuel = currentPos.remainFuel - costFuel;
	//检查是否重复
	MovePoint *exist=nullptr;
	for(auto &mp:movablePoints){
		if(mp.x==p.x && mp.y==p.y){
			exist=&mp;//嗯貌似别的路径也能走过这个点，那么应该找最优
			break;
		}
	}
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
	for(auto itr=movePath.begin();itr!=movePath.end();++itr){
		if(p==*itr){//在路径内,那么需要判断路径是否需要发生变化
			++itr;
			if(itr==movePath.end())return false;
			//删除多余的路径
			while(itr!=movePath.end())itr=movePath.erase(itr);
			return true;
		}
	}
	//--检查目标点在不在移动范围内
	for(auto &mp:movablePoints){
		if(p==mp){
			//试图移动到新的点
			if(tryToPathTo(mp))return false;
			else{//不能沿着新的路径移动过去，那么我们就调整路径
				movePath.clear();
				movePath.push_front(mp);
				auto pp = movePath.front();
				while(true){
					if(tryToBackPathTo(pp,pUp));
					else if(tryToBackPathTo(pp,pDown));
					else if(tryToBackPathTo(pp,pLeft));
					else if(tryToBackPathTo(pp,pRight));
					else break;
					pp = movePath.front();
				}
			}
			break;
		}
	}
	return true;
}
bool Campaign::tryToPathTo(const MovePoint &target){
	//这个点必须和路径的最后一个点是相邻的
	auto lastP=movePath.rbegin();
	CoordType delta=target-(*lastP);
	if(delta.manhattanLength()!=1)return false;
	//取地形
	if(!cursorTerrainCode)return false;
	//看看剩余的移动力能不能过去
	int moveCost=luaFunc_movementCost(selectCorp->moveType,cursorTerrainCode->name,"Normal");
	if(lastP->remainMovement < moveCost)return false;
	//看看剩余的燃料能不能过去
	int costFuel=luaFunc_fuelCost("Normal");
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
bool Campaign::moveWithPath(){
	for(auto &path:movePath){
		//碰撞检测，检测是否发生碰撞
		for(auto &unit:battleField->chessPieces){
			if(unit.coordinate==path && unit.color!=selectUnit->color){
				auto team=findTeam(unit.color);//判断是敌人还是朋友,不是朋友的一般都会采取拦截措施
				if(!team)return false;
				if(!team->friendsTeams.contain(selectUnit->color))return false;
			}
		}
		//无碰撞,执行移动过程
		selectUnit->coordinate=path;
		selectUnit->fuel=path.remainFuel;
	}
	return true;
}

void Campaign::caculateFlightshot_byMovement(){//根据移动范围来计算射程
	firablePoints.clear();
	for(auto &p:movablePoints){
		firablePoints.push_back(p+pUp);
		firablePoints.push_back(p+pDown);
		firablePoints.push_back(p+pLeft);
		firablePoints.push_back(p+pRight);
	}
	firablePoints.unique();
	//移除超出地图的点
	caculateFlightshot_removeNotInRange();
	//移除和移动范围一样的点
	for(auto &p:movablePoints){
		firablePoints.remove(p);
	}
}
void Campaign::caculateFlightshot_byCenter(const CoordType &center,int minDistance,int maxDistance){
	firablePoints.clear();
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
	auto itr=firablePoints.begin();
	while(itr!=firablePoints.end()){
		if(battleField->isInRange(itr->x,itr->y)){
			++itr;
		}else{
			itr=firablePoints.erase(itr);
		}
	}
}

void Campaign::cursorSelect(){
	if(isEditMode){//地图编辑模式
	}else{
		if(!selectUnit){//未选择单位的时候
			if(cursorUnit){//如果选择了单位,则计算移动范围
				selectUnit=cursorUnit;
				selectCorp=cursorCorp;
				selectTeam=cursorTeam;
				caculateMovement(*cursorUnit);
			}else if(cursorTerrainCode){//如果没有单位,则有可能是制造单位的工厂
				if(cursorTerrainCode->buildable){//可建造部队,那么我们需要显示菜单
					//printf("%s可生产部队\n",cursorTerrainCode->name.data());
					produceMenu.clear();
					string corpType;//可生产的兵种类型
					const string &name(cursorTerrainCode->name);
					if(name=="Factory")corpType="LandForce";
					else if(name=="AirFactory")corpType="AirForce";
					else if(name=="ShipFactory")corpType="NavyForce";
					for(auto &corp:*battleField->corpsList){
						if(corpType==corp.corpType){
							//produceMenu.push_back(corp.id);
						}
					}
				}
			}
		}else{//选择单位并点击无单位的地方,可能是移动,也可能是取消
			if(*movePath.rbegin()==cursor){//在移动范围内,弹出菜单
				showCorpMenu();
			}else{//不在移动范围内,清除移动范围
				cursorCancel();
			}
		}
	}
}
void Campaign::cursorCancel(){
	//清除移动范围
	movablePoints.clear();
	movePath.clear();
	firablePoints.clear();
	corpMenu.clear();
	produceMenu.clear();
	//刷新
	selectUnit=nullptr;
	setCursor(cursor.x,cursor.y);
}
void Campaign::executeMenuSelect(int index){
	if(corpMenu.size()){
		executeCorpMenu(index);
	}else if(produceMenu.size()){
		executeProduceMenu(index);
	}
}
void Campaign::executeCorpMenu(int index){
	//根据idx确定要执行的command
	int idx=0,command=-1;
	for(auto &item:corpMenu){
		if(idx==index){
			command=item;
			break;
		}
		++idx;
	}
	if(command<0)return;
	//执行
	switch(command){
#define CAMPAIGN_COMMAND(name) case Menu_##name:execMenuItem_##name();return;
		CAMPAIGN_COMMAND(Fire)//需要选择攻击目标
		CAMPAIGN_COMMAND(Drop)//需要选择单位和卸载点
		CAMPAIGN_COMMAND(Flare)//需要选择闪光点
		CAMPAIGN_COMMAND(Launch)//需要选择发射目标
		CAMPAIGN_COMMAND(Repair)//需要选择修理目标
#undef CAMPAIGN_COMMAND
		default://以上指令的需要一些额外信息,但是以下指令则不需要
			if(moveWithPath()){//没有发生碰撞的情况下,才执行指令
				switch(command){
#define CAMPAIGN_COMMAND(name) case Menu_##name:execMenuItem_##name();break;
					CAMPAIGN_COMMAND(Capture)
					CAMPAIGN_COMMAND(Join)
					CAMPAIGN_COMMAND(Load)
					CAMPAIGN_COMMAND(Supply)
					CAMPAIGN_COMMAND(Build)
					CAMPAIGN_COMMAND(Hide)
					CAMPAIGN_COMMAND(Appear)
					CAMPAIGN_COMMAND(Explode)
					CAMPAIGN_COMMAND(Wait)
#undef CAMPAIGN_COMMAND
					default:;
				}
			}
	}
	cursorCancel();//清理缓冲
	//这里需要调用一个刷新视野的函数
}
void Campaign::executeProduceMenu(int index){
	//根据idx确定要执行的command
	int idx=0,command=-1;
	for(auto &item:produceMenu){
		if(idx==index){
			command=item;
			break;
		}
		++idx;
	}
	if(command<0)return;
	//确定了兵种,我们可以进行生产
	printf("生产单位%d,%d\n",cursor.x,cursor.y);
	Unit unit;
	unit.corpType=command;
	unit.color=cursorTerrain.status;
	unit.coordinate=cursor;
	battleField->chessPieces.push_back(unit);
	cursorCancel();//清理缓冲
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
		corpMenu.push_back(Menu_Wait);
	}
}
bool Campaign::showMenuItem_Join(){
	if(cursorUnit){
		if(selectCorp!=cursorCorp)return false;//兵种相同才能结合
		if(selectTeam!=cursorTeam)return false;//同一支部队的才能结合
		if(selectUnit==cursorUnit)return false;//不能自己与自己结合
		if(cursorUnit->presentHP() >= 10)return false;//被结合方的表现hp要小于10
		corpMenu.push_back(Menu_Join);
		return true;
	}else return false;
}
bool Campaign::showMenuItem_Load(){
	if(cursorUnit){
		if(selectTeam!=cursorTeam)return false;//同一支部队的才能装载？
		if(selectUnit==cursorUnit)return false;//不能自己与自己结合
		SizeType amount=luaFunc_loadableAmount(cursorCorp->name,selectCorp->name);
		if(amount>0 && cursorUnit->loadedUnits.size()<amount){//如果可装载,并且有空位
			corpMenu.push_back(Menu_Load);
			return true;
		}
	}
	return false;
}
bool Campaign::showMenuItem_Drop(){
	auto canStayAt=[=](const string &moveType,const CoordType &p,bool checkBarrier){
		Terrain terrain;
		if(!battleField->getTerrain(p.x,p.y,terrain))return false;//p要在地图范围内
		auto trnCode=battleField->terrainsList->data(terrain.terrainType);
		if(!trnCode)return false;//必须有地形信息
		int moveCost=luaFunc_movementCost(moveType,trnCode->name,"Normal");
		if(moveCost<0)return false;//必须是可停靠地形
		if(checkBarrier){
			for(auto &barrier:battleField->chessPieces){
				if(barrier.coordinate==p && barrier.isVisible)return false;//有障碍单位,不能停
			}
		}
		return true;
	};
	//首先,查看自己有没有部队可以卸载
	dropableUnits.clear();
	for(auto &unit:selectUnit->loadedUnits){
		auto corp=battleField->corpsList->data(unit.corpType);
		if(!corp)return false;
		if(canStayAt(corp->moveType,cursor,false)){//检测落点是否可走
			if(canStayAt(corp->moveType,cursor+pUp,true)
			|| canStayAt(corp->moveType,cursor+pDown,true)
			|| canStayAt(corp->moveType,cursor+pLeft,true)
			|| canStayAt(corp->moveType,cursor+pRight,true)){
				dropableUnits.push_back(&unit);
			}
		}
	}
	//有可卸载部队就显示菜单项
	if(dropableUnits.size()){
		corpMenu.push_back(Menu_Drop);
	}
	return dropableUnits.size();
}
bool Campaign::showMenuItem_Fire(){
	//需要从lua文件中查询出关系
	return false;
}
bool Campaign::showMenuItem_Capture(){
	if(!selectCorp->capturable)return false;//兵种要具有占领能力
	if(!cursorTerrainCode->capturable)return false;//目标地形必须是可占领的据点
	if(selectUnit->color == cursorTerrain.status)return false;//自己不能占领自己
	if(selectTeam){
		if(selectTeam->friendsTeams.contain(cursorTerrain.status))return false;//自己不能占领友军的据点
		if(!selectTeam->enemiesTeams.contain(cursorTerrain.status)){//如果是非敌对势力的话....(比如中立据点)
			if(findTeam(cursorTerrain.status))return false;//只要不在参赛列表中就可以占领
		}
	}
	corpMenu.push_back(Menu_Capture);
	return true;
}
bool Campaign::showMenuItem_Supply(){
	if(!selectCorp->suppliable)return false;//兵种要具有补给能力
	suppliableUnits.clear();
	auto &unitList(battleField->chessPieces);
	for(auto &unit:unitList){
		if((cursor-unit.coordinate).manhattanLength()==1 && selectUnit!=&unit){//首先必须位置相邻,且不是自己
			if(selectTeam->teamID==unit.color || selectTeam->friendsTeams.contain(unit.color)){//必须是自军或者友军
				suppliableUnits.push_back(&unit);
			}
		}
	}
	if(suppliableUnits.size()){//有可补给的单位就显示此指令
		corpMenu.push_back(Menu_Supply);
	}
	return suppliableUnits.size();
}
bool Campaign::showMenuItem_Build(){
	if(!selectCorp->buildable)return false;//兵种要具有建造能力
	//需要从lua文件中查询出关系
	return false;
}
bool Campaign::showMenuItem_Flare(){
	if(!selectCorp->flarable)return false;//兵种要具有闪光能力
	if(selectUnit->coordinate!=cursor)return false;//发射时候不能移动
	if(selectUnit->ammunition <=0)return false;//没弹药不能发射
	corpMenu.push_back(Menu_Flare);
	return true;
}
bool Campaign::showMenuItem_Hide(){
	if(!selectCorp->hidable)return false;//兵种要具有隐藏能力
	if(selectUnit->isHide)return false;//已经隐藏的兵种不能再次隐藏
	corpMenu.push_back(Menu_Hide);
	return true;
}
bool Campaign::showMenuItem_Appear(){
	if(!selectCorp->hidable)return false;//兵种要具有隐藏能力
	if(!selectUnit->isHide)return false;//未隐藏的兵种不能显示
	corpMenu.push_back(Menu_Appear);
	return true;
}
bool Campaign::showMenuItem_Launch(){
	if(selectCorp->capturable && cursorTerrainCode->name=="Silo"){//主要是占领系的兵种可以发射火箭
		corpMenu.push_back(Menu_Launch);
		return true;
	}else return false;
}
bool Campaign::showMenuItem_Repair(){
	if(!selectCorp->repairable)return false;//兵种要具有隐藏能力
	suppliableUnits.clear();
	auto &unitList(battleField->chessPieces);
	for(auto &unit:unitList){
		if((cursor-unit.coordinate).manhattanLength()==1 && selectUnit!=&unit){//首先必须位置相邻,且不是自己
			if(selectTeam->teamID==unit.color || selectTeam->friendsTeams.contain(unit.color)){//必须是自军或者友军
				suppliableUnits.push_back(&unit);
			}
		}
	}
	if(suppliableUnits.size()){//有可补给的单位就显示此指令
		corpMenu.push_back(Menu_Repair);
	}
	return suppliableUnits.size();
}
bool Campaign::showMenuItem_Explode(){
	if(!selectCorp->explodable)return false;//兵种要具有隐藏能力
	corpMenu.push_back(Menu_Explode);
	return true;
}
bool Campaign::showMenuItem_Wait(){
	return (!cursorUnit || cursorUnit==selectUnit);//移动到无单位的位置,或者原地待机时候可显示
}

bool Campaign::execMenuItem_Join(){
	//燃料
	cursorUnit->fuel += selectUnit->fuel;//累加燃料
	if(cursorUnit->fuel>cursorCorp->gasMax)cursorUnit->fuel=cursorCorp->gasMax;//燃料不超过上限
	//弹药
	for(auto &wpn:cursorCorp->weapons){
		if(wpn.ammunitionMax){
			cursorUnit->ammunition += selectUnit->ammunition;//累加弹药
			if(cursorUnit->ammunition > wpn.ammunitionMax)cursorUnit->ammunition = wpn.ammunitionMax;//弹药不超过上线
			break;
		}
	}
	//HP
	/*int presentHP = selectUnit->presentHP() + cursorUnit->presentHP();
	int factHP = selectUnit->healthPower + cursorUnit->healthPower;
	if(presentHP>10){//处理表现HP超出10的情况
		int pastHP=presentHP-10;//多出来的表现HP
		presentHP=10;
		selectTeam->funds += selectCorp->price * pastHP /10;//我们应该转换成价值
	}
	//调整一下精确的hp
	if(presentHP * 10 < factHP)factHP = presentHP*10;//不超过上限
	else if(presentHP * 10 > factHP)factHP = (presentHP-1)*10+1;//不低于下限
	cursorUnit->healthPower = factHP;
	//要删除掉selectUnit,以cursorUnit的状态为准
	battleField->chessPieces.removeRef(*selectUnit);*/
	return false;
}
bool Campaign::execMenuItem_Load(){
	cursorUnit->loadedUnits.push_back(*selectUnit);
	//battleField->chessPieces.removeRef(*selectUnit);
	return true;
}
bool Campaign::execMenuItem_Drop(){
	return false;
}
bool Campaign::execMenuItem_Fire(){return false;}
bool Campaign::execMenuItem_Capture(){return false;}
bool Campaign::execMenuItem_Supply(){return false;}
bool Campaign::execMenuItem_Build(){return false;}
bool Campaign::execMenuItem_Flare(){return false;}
bool Campaign::execMenuItem_Hide(){return false;}
bool Campaign::execMenuItem_Appear(){return false;}
bool Campaign::execMenuItem_Launch(){return false;}
bool Campaign::execMenuItem_Repair(){return false;}
bool Campaign::execMenuItem_Explode(){return false;}
bool Campaign::execMenuItem_Wait(){
	if(selectUnit && !selectUnit->isWait){
		selectUnit->isWait=true;
		return true;
	}else return false;
}
