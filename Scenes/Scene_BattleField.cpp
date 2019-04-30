#include"Scene_BattleField.h"
#include"Game_AdvanceWars.h"

#include"ShapeRenderer.h"

static ShapeRenderer sr;//绘制边框用
static int latticeSize=32;//格子大小

//static GameMenu_String corpMenu;//兵种命令菜单
//static Scene_DataTable scene_DataTable;//数据表场景,生产部队时候可以进入此场景
static CorpsList producableCorpsList;//可生产的兵种表
//菜单项与文字的对应关系
static const char* corpMenuItemName[Campaign::AmountOfEnumMenu];

Scene_BattleField::Scene_BattleField():terrainsTextures(nullptr),corpsTextures(nullptr){
#define ITEMNAME(name) corpMenuItemName[Campaign::Menu_##name]=#name;
	CAMPAIGN_CORPMENU(ITEMNAME)
#undef ITEMNAME
}
Scene_BattleField::~Scene_BattleField(){}

void Scene_BattleField::keyboardKey(Keyboard::KeyboardKey key,bool pressed){
	if(campaign->corpMenu.size()){//如果显示了菜单,则把事件直接传给菜单
		//corpMenu.keyboardKey(key,pressed);
		return;
	}
	bool isDirectionKey=
		(key==Keyboard::Key_Up)||
		(key==Keyboard::Key_Down)||
		(key==Keyboard::Key_Left)||
		(key==Keyboard::Key_Right);
	if(isDirectionKey && !pressed){
		auto x=campaign->cursor.x();
		auto y=campaign->cursor.y();
		switch(key){
			case Keyboard::Key_Up:
				if(y>0)--y;
			break;
			case Keyboard::Key_Down:
				if(y<(int)battleField->getHeight()-1)++y;
			break;
			case Keyboard::Key_Left:
				if(x>0)--x;
			break;
			case Keyboard::Key_Right:
				if(x<(int)battleField->getWidth()-1)++x;
			break;
			default:;
		}
		campaign->setCursor(x,y);
		getCursorInfo();//获取信息
	}else if(key==Keyboard::Key_Enter && !pressed){
		cursorSelect();//处理选择事件
	}else if(key==Keyboard::Key_F1 && !pressed){
	}
	//根据光标位置改变场景的镜头位置
}

//拖动效果变量
static decltype(Game::mousePos) touchBeginPoint,touchMovePoint;//触摸起点,移动中的点
static bool mouseKeyDown=false;//是否按下鼠标
static bool isTouchMove=false;//是否拖动行为

void Scene_BattleField::mouseKey(MouseKey key,bool pressed){
	if(campaign->corpMenu.size()){//如果显示了菜单,则把事件直接传给菜单
		//corpMenu.mouseKey(key,pressed);
		return;
	}
	if(key==Mouse_LeftButton){
		if(pressed){//点击时候记录位置
			touchBeginPoint = Game::mousePos;
			touchMovePoint = Game::mousePos;
		}else if(isTouchMove);//如果是拖动,放手后一般不处理
		else{//处理点击事件
			cursorSelect();
		}
		//改变状态
		mouseKeyDown=pressed;
		isTouchMove=false;
	}
}
void Scene_BattleField::mouseMove(int x,int y){
	if(campaign->corpMenu.size()){//如果显示了菜单,则把事件直接传给菜单
		//corpMenu.mouseMove(x,y);
		return;
	}
	if(mouseKeyDown){//有可能是拖动
		if(isTouchMove){//移动地图
			//改变位置
			position.x()-=(x-touchMovePoint.x());
			position.y()-=(y-touchMovePoint.y());
			//进行可靠修正
			//fixPosition();
		}else{//判断移动距离是不是很大,是的话切换到移动模式
			if((touchBeginPoint-Game::mousePos).distance2()>=latticeSize*latticeSize){
				isTouchMove=true;
			}
		}
		touchMovePoint.x()=x;
		touchMovePoint.y()=y;
	}else{//鼠标移动
		auto res=Game::resolution;
		auto fieldLen=sizeF();
		if(fieldLen.x()<=res.x()){//宽度没超出分辨率
			x-=(res.x()-fieldLen.x())/2;
		}else{
			x+=position.x();
		}
		if(fieldLen.y()<=res.y()){//高度没超出分辨率
			y-=(res.y()-fieldLen.y())/2;
		}else{
			y+=position.y();
		}
		setCursorPos(x,y);
	}
}

void Scene_BattleField::render()const{
	GameScene::render();
	int w=battleField->getWidth();
	int h=battleField->getHeight();
	//我们先确定当前地图有多大(像素)
	auto res=Game::resolution;
	auto fieldLen=sizeF(),pos=posF(),renderPos=pos;
	Point2D<int> minXY,maxXY;
	//确定渲染区域(像素)
	if(fieldLen.x()<=res.x()){//宽度没超出分辨率
		renderPos.x()=(res.x()-fieldLen.x())/2;//水平居中
		minXY.x()=0;
		maxXY.x()=w;
	}else{//宽度超出分辨率
		renderPos.x()=-((int)pos.x()%latticeSize);//渲染起点
		minXY.x()=pos.x()/latticeSize;
		maxXY.x()=(pos.x()+res.x())/latticeSize + ((int)(pos.x()+res.x())%latticeSize?1:0);
		if(maxXY.x()>w)maxXY.x()=w;
	}
	if(fieldLen.y()<=res.y()){//高度没超出分辨率
		renderPos.y()=(res.y()-fieldLen.y())/2;//垂直居中
		minXY.y()=0;
		maxXY.y()=h;
	}else{//高度超出分辨率
		renderPos.y()=-((int)pos.y()%latticeSize);//渲染起点
		minXY.y()=h-((int)(pos.y()+res.y()))/latticeSize - ((int)(pos.y()+res.y())%latticeSize?1:0);
		maxXY.y()=h-(int)pos.y()/latticeSize;
		if(maxXY.y()>h)maxXY.y()=h;
	}
	sr.hasFill=false;
	sr.edgeColor=ColorRGBA(0,0,255,128);
	//画地形图块
	if(!terrainsTextures)return;
	Terrain terrain;
	for(int x=minXY.x();x<maxXY.x();++x){
		for(int y=minXY.y();y<maxXY.y();++y){
			if(battleField->getTerrain(x,y,terrain)){
				//画地形
				auto tex=terrainsTextures->value(terrain.terrainType+terrain.status*256);
				if(tex){
					glColor4ub(color.red,color.green,color.blue,color.alpha);
					tex->draw(Point2D<float>(
						renderPos.x()+(x-minXY.x())*latticeSize,
						renderPos.y()+(maxXY.y()-1-y)*latticeSize),
						Point2D<float>(tex->getWidth()*2,tex->getHeight()*2));
				}
			}
			//画网格
			sr.drawRectangle(
				renderPos.x()+(x-minXY.x())*latticeSize+1,
				renderPos.y()+(maxXY.y()-1-y)*latticeSize+1,
				renderPos.x()+(x-minXY.x()+1)*latticeSize,
				renderPos.y()+(maxXY.y()-y)*latticeSize);
		}
	}
	//画移动范围
	sr.hasFill=true;
	sr.fillColor=ColorRGBA(0,0,255,128);
	for(auto &p:campaign->movablePoints){
		int x=p.x(),y=p.y();
		if(p.x()>=minXY.x() && p.x()<maxXY.x() && p.y()>=minXY.y() && p.y()<maxXY.y()){
			sr.drawRectangle(
				renderPos.x()+(x-minXY.x())*latticeSize+1,
				renderPos.y()+(maxXY.y()-1-y)*latticeSize+1,
				renderPos.x()+(x-minXY.x()+1)*latticeSize,
				renderPos.y()+(maxXY.y()-y)*latticeSize);
		}
	}
	//画移动路径
	sr.hasFill=true;
	sr.fillColor=ColorRGBA(0,255,0,128);
	for(auto &p:campaign->movePath){
		int x=p.x(),y=p.y();
		if(p.x()>=minXY.x() && p.x()<maxXY.x() && p.y()>=minXY.y() && p.y()<maxXY.y()){
			sr.drawRectangle(
				renderPos.x()+(x-minXY.x())*latticeSize+1,
				renderPos.y()+(maxXY.y()-1-y)*latticeSize+1,
				renderPos.x()+(x-minXY.x()+1)*latticeSize,
				renderPos.y()+(maxXY.y()-y)*latticeSize);
		}
	}
	//画攻击范围
	sr.hasFill=true;
	sr.fillColor=ColorRGBA(255,0,0,128);
	for(auto &p:campaign->firablePoints){
		int x=p.x(),y=p.y();
		if(p.x()>=minXY.x() && p.x()<maxXY.x() && p.y()>=minXY.y() && p.y()<maxXY.y()){
			sr.drawRectangle(
				renderPos.x()+(x-minXY.x())*latticeSize+1,
				renderPos.y()+(maxXY.y()-1-y)*latticeSize+1,
				renderPos.x()+(x-minXY.x()+1)*latticeSize,
				renderPos.y()+(maxXY.y()-y)*latticeSize);
		}
	}
	//画单位
	glColor4ub(color.red,color.green,color.blue,color.alpha);
	for(auto &unit:battleField->chessPieces){
		auto p=unit.coordinate;
		if(p.x()<minXY.x()||p.x()>=maxXY.x())continue;
		if(p.y()<minXY.y()||p.y()>=maxXY.y())continue;
		//画单位
		auto tex=corpsTextures->value(unit.corpType + unit.color*256);
		if(tex){
			tex->draw(Point2D<float>(
				renderPos.x()+(p.x()-minXY.x())*latticeSize,
				renderPos.y()+(maxXY.y()-1-p.y())*latticeSize));
		}
	}
	//画光标
	sr.hasFill=false;
	sr.edgeColor=ColorRGBA(255,0,0,255);
	auto curPos=cursorPos();
	//计算绘画位置
	int x=curPos.x()+1-position.x();
	int y=curPos.y()+1-position.y();
	if(fieldLen.x()<=res.x())x=curPos.x()+1+renderPos.x();
	if(fieldLen.y()<=res.y())x=curPos.y()+1+renderPos.y();
	//开始绘制
	sr.drawTriangle(x,y,x+8,y,x,y+8);//左下
	x+=latticeSize-1;
	sr.drawTriangle(x,y,x-8,y,x,y+8);//右下
	y+=latticeSize-1;
	sr.drawTriangle(x,y,x-8,y,x,y-8);//右上
	x-=latticeSize-1;
	sr.drawTriangle(x,y,x+8,y,x,y-8);//左上

	//看情况显示兵种命令
	/*if(corpMenu.rowAmount()){
		//放在合适的位置来显示
		corpMenu.position=Game::resolution/2;
		corpMenu.color.alpha=255;
		corpMenu.render();
	}else{
		corpMenu.clearAllItems();
		corpMenu.color.alpha=0;
		//显示地形信息
		renderTerrainInfo();
		renderUnitInfo();
	}*/
}
Point2D<float> Scene_BattleField::sizeF()const{
	decltype(sizeF()) ret;
	if(battleField){
		ret.x()=battleField->getWidth()*latticeSize;
		ret.y()=battleField->getHeight()*latticeSize;
	}
	return ret;
}
void Scene_BattleField::consumeTimeSlice(){
	//处理兵种菜单操作
	/*switch(corpMenu.menuStatus){
		case GameMenu::Confirm://菜单确认,则执行菜单命令
			corpMenu.menuStatus=GameMenu::Selecting;
			campaign->executeMenuSelect(corpMenu.selectingItem);
		break;
		case GameMenu::Cancel://取消菜单操作
			corpMenu.menuStatus=GameMenu::Selecting;
			campaign->corpMenu.clear();
		break;
		default:;
	}
	if(!campaign->corpMenu.size()){//实时更新菜单缓存
		corpMenu.clearAllItems();
	}
	//显示生产菜单
	auto &scene_DataTable=(Game_AdvanceWars::currentGame()->scene_DataTable);
	if(campaign->produceMenu.size() && producableCorpsList.size()==0){
		printf("显示生产界面\n");
		scene_DataTable.setTableType(Game_AdvanceWars::File_Corps);
		scene_DataTable.gameString_Title.setString("生产单位");
		scene_DataTable.tableCorpData.source=&producableCorpsList;
		scene_DataTable.tableCorpData.textures=corpsTextures;
		scene_DataTable.tableCorpData.troopID=campaign->cursorTerrain.status;
		//添加菜单项
		int idx=0;
		for(auto &corp:*battleField->corpsList){
			if(campaign->produceMenu.contain(idx)){//根据可生产的兵种来添加
				producableCorpsList.push_back(corp);
			}
			++idx;
		}
		Game::game->subObjects.push_front(&scene_DataTable);//场景切换
	}
	//处理生产指令
	switch(scene_DataTable.tableCorpData.menuStatus){
		case GameMenu::Confirm:
			//在地图上创建单位
			campaign->executeMenuSelect(scene_DataTable.tableCorpData.selectingItem);
		//break;执行和取消一样的清理操作
		case GameMenu::Cancel://取消生产单位
			scene_DataTable.tableCorpData.selectingItem=0;
			scene_DataTable.tableCorpData.renderItemFrom=0;
			scene_DataTable.tableCorpData.menuStatus=GameMenu::Selecting;
			producableCorpsList.clear();
		break;
		default:;
	}*/
}

//地形信息显示
static Texture *terrainIcon=nullptr;//地形图标
static GameString gameString_terrainCoord;//地形坐标
static GameString gameString_terrainName;//显示地形名
static GameString gameString_terrainDefend;//地形防御

void Scene_BattleField::renderTerrainInfo()const{
	/*if(!battleField)return;
	//判断光标位置
	decltype(position) pos=cursorPos()-position;
	if(pos.x()<Game::resolution.x()/2){
		pos.x()=Game::resolution.x()-256-32;
	}else{
		pos.x()=32;
	}
	//画方框
	sr.hasFill=true;
	sr.fillColor=ColorRGBA(0,0,0,192);
	sr.drawRectangle(pos.x(),0,pos.x()+256,112);
	//画地形图标
	if(terrainIcon){
		glColor4ub(color.red,color.green,color.blue,color.alpha);
		terrainIcon->draw(Point2D<float>(pos.x()+8,8),
			Point2D<float>(terrainIcon->getWidth()*2,terrainIcon->getHeight()*2));
	}
	//画文字
	gameString_terrainCoord.anchorPoint=Point2D<float>(0,0);
	gameString_terrainCoord.position=Point2D<int>(pos.x()+0,72);
	gameString_terrainCoord.render();
	gameString_terrainDefend.anchorPoint=Point2D<float>(0,0);
	gameString_terrainDefend.position=Point2D<int>(pos.x()+40,40);
	gameString_terrainDefend.render();
	gameString_terrainName.anchorPoint=Point2D<float>(0,0);
	gameString_terrainName.position=Point2D<int>(pos.x()+40,8);
	gameString_terrainName.render();*/
}
//单位信息显示
static Texture *unitIcon=nullptr;//单位图标
static GameString gameString_unitName;//显示单位名
static GameString gameString_unitHP;//显示单位HP
static GameString gameString_unitGasAndAmmu;//显示汽油弹药

void Scene_BattleField::renderUnitInfo()const{
	/*if(!battleField || !unitIcon)return;
	//判断光标位置
	decltype(position) pos=cursorPos()-position;
	if(pos.x()<Game::resolution.x()/2){
		pos.x()=Game::resolution.x()-256-32;
	}else{
		pos.x()=32;
	}
	//画方框
	sr.hasFill=true;
	sr.fillColor=ColorRGBA(0,0,0,192);
	sr.drawRectangle(pos.x(),112,pos.x()+256,224);
	//画单位
	if(unitIcon){
		glColor4ub(color.red,color.green,color.blue,color.alpha);
		unitIcon->draw(Point2D<float>(pos.x()+8,120),
			Point2D<float>(unitIcon->getWidth(),unitIcon->getHeight()));
	}
	//画文字
	gameString_unitName.anchorPoint=Point2D<float>(0,0);
	gameString_unitName.position=Point2D<int>(pos.x()+40,120);
	gameString_unitName.render();
	gameString_unitHP.anchorPoint=Point2D<float>(0,0);
	gameString_unitHP.position=Point2D<int>(pos.x()+40,152);
	gameString_unitHP.render();
	gameString_unitGasAndAmmu.anchorPoint=Point2D<float>(0,0);
	gameString_unitGasAndAmmu.position=Point2D<int>(pos.x(),184);
	gameString_unitGasAndAmmu.render();*/
}
void Scene_BattleField::renderTeamInfo()const{
	//判断光标位置
	/*decltype(position) pos=cursorPos()-position;
	if(pos.x()<Game::resolution.x()/2){
		pos.x()=Game::resolution.x()-256-32;
	}else{
		pos.x()=32;
	}
	//画方框
	sr.hasFill=true;
	sr.fillColor=ColorRGBA(0,0,0,192);
	sr.drawRectangle(pos.x(),112,pos.x()+256,224);*/
}

Point2D<int> Scene_BattleField::cursorPos()const{
	decltype(cursorPos()) ret;
	if(battleField){
		auto h=battleField->getHeight();
		ret.x()=campaign->cursor.x()*latticeSize;
		ret.y()=(h-1-campaign->cursor.y())*latticeSize;
	}
	return ret;
}
void Scene_BattleField::setCursorPos(int x,int y){
	if(battleField){
		//设置坐标
		auto h=battleField->getHeight();
		x=x/latticeSize;
		y=h-1-y/latticeSize;
		if(campaign->cursor==decltype(Unit::coordinate)(x,y))return;
		campaign->setCursor(x,y);
		getCursorInfo();//获取信息
	}
}
void Scene_BattleField::cursorSelect(){
	/*campaign->cursorSelect();
	if(campaign->corpMenu.size()){//有菜单项,显示菜单项
		corpMenu.clearAllItems();
		for(auto &item:campaign->corpMenu){
			corpMenu.insertItem(corpMenuItemName[item]);
		}
		//根据项数调整大小
		auto amount=corpMenu.rowAmount();
		decltype(amount) maxAmount=4;
		corpMenu.renderItemAmount=(amount>maxAmount?maxAmount:amount);
		corpMenu.refresh();
		corpMenu.selectingItem=0;
	}*/
}
void Scene_BattleField::getCursorInfo()const{
	//获取对应坐标的地形
	Terrain &terrain(campaign->cursorTerrain);
	terrainIcon=terrainsTextures->value(terrain.terrainType+terrain.status*256);//取得地形纹理
	auto code=battleField->terrainsList->data(terrain.terrainType);//取得地形名字
	if(code){
		gameString_terrainName.setString(code->name);
		char str[30];
		sprintf(str,"防御%d",code->defendLV);
		gameString_terrainDefend.setString(str);
		sprintf(str,"X=%d,Y=%d",campaign->cursor.x(),campaign->cursor.y());
		gameString_terrainCoord.setString(str);
	}
	//获取单位信息
	unitIcon=nullptr;
	if(campaign->cursorUnit){
		Unit &unit(*campaign->cursorUnit);
		unitIcon=corpsTextures->value(unit.corpType + unit.color*256);
		auto code=battleField->corpsList->data(unit.corpType);
		if(code){
			gameString_unitName.setString(code->name);
			char str[30];
			sprintf(str,"HP=%d(%d)",unit.presentHP(),unit.healthPower);
			gameString_unitHP.setString(str);
			sprintf(str,"汽油=%d,弹药=%d",unit.fuel,unit.ammunition);
			gameString_unitGasAndAmmu.setString(str);
		}
	}
}
