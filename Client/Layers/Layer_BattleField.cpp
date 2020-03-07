#include"Layer_BattleField.h"
#include"Sprite_Unit.h"
#include"Game_AdvanceWars.h"

static int latticeSize=32;//格子大小
//渲染变量
static Rectangle2D<int> mapRect;//地图矩形
static Point2D<float> renderPos;//渲染位置
static Rectangle2D<float> renderLattice;//渲染格子

static Sprite_Unit spriteUnit;//单位精灵,用于渲染单位
static Texture texCursor;//光标纹理,用于渲染光标

static ColorRGBA rgbMove,rgbFire,rgbMovePath;//移动区域的颜色,可攻击区域的颜色,移动路径颜色

static void updateRenderPos(int x,int y){//更新渲染位置,根据x,y来更新renderPos
	renderPos.setXY(mapRect.p0.x + x*latticeSize,mapRect.p0.y + y*latticeSize);
}
static void updateRenderLattice(int x,int y){//更新需要渲染的网格数据
	updateRenderPos(x,y);
	renderLattice.p0=renderPos;
	renderLattice.p1.setXY(renderLattice.p0.x +latticeSize-1,renderLattice.p0.y +latticeSize-1);
}

Layer_BattleField::Layer_BattleField():battleField(nullptr),campaign(nullptr),
isEditMode(false),isEditMode_Unit(false),terrainID(0),corpID(0),troopID(0){
	GAME_AW
	battleField=&game->battleField;//读取数据用
	campaign=&game->campaign;
	//精灵初始化
	spriteUnit.anchorPoint.setXY(0,0);
	spriteUnit.texArray=&game->corpsTexturesArray;//渲染单位用
	texCursor.texImage2D_FilePNG(game->settings.imagesPathIcons+"/Cursor.png",Game::whenError);
	//颜色设定
	rgbMove.blue=255;rgbMove.alpha=128;
	rgbFire.red=255;rgbFire.alpha=128;
	rgbMovePath.green=255;rgbMovePath.alpha=128;
}
Layer_BattleField::~Layer_BattleField(){}

bool Layer_BattleField::keyboardKey(Keyboard::KeyboardKey key,bool pressed){
	if(!pressed){
		int x=0,y=0;
		switch(key){
			case Keyboard::Key_Up:y=1;break;
			case Keyboard::Key_Down:y=-1;break;
			case Keyboard::Key_Left:x=-1;break;
			case Keyboard::Key_Right:x=1;break;
			case Keyboard::Key_Enter:whenPressConfirm();break;
			case Keyboard::Key_Esc:whenPressCancel();break;
			default:;
		}
		if(x || y){//方向键,进行移动
			campaign->setCursor(campaign->cursor + Campaign::CoordType(x,y));
		}
	}
	return forceIntercept;
}

//拖动效果变量
static decltype(Game::mousePos) touchBeginPoint,touchMovePoint;//触摸起点,移动中的点
static bool mouseKeyDown=false;//是否按下鼠标
static bool isTouchMove=false;//是否拖动行为

bool Layer_BattleField::mouseKey(MouseKey key,bool pressed){
	if(!pressed){
		switch(key){
			case Mouse_LeftButton:whenPressConfirm();break;
			case Mouse_RightButton:whenPressCancel();break;
			default:;
		}
	}
	return forceIntercept;
}
bool Layer_BattleField::mouseMove(int x,int y){
	if(mouseKeyDown){//有可能是拖动
		if(isTouchMove){//移动地图
			//改变位置
			position.x+=(x-touchMovePoint.x);
			position.y+=(y-touchMovePoint.y);
		}else{//判断移动距离是不是很大,是的话切换到移动模式
			if((touchBeginPoint-Game::mousePos).distance2()>=latticeSize*latticeSize){
				isTouchMove=true;
			}
		}
		touchMovePoint.x=x;
		touchMovePoint.y=y;
	}else{//鼠标移动
		//计算出鼠标对应的地图格子坐标
		GAME_AW
		auto pos=game->mousePos-(mapRect.p0 + position);
		cursorPoint.setXY(pos.x/latticeSize,pos.y/latticeSize);
		//判断是否需要更新
		if(cursorPoint!=campaign->cursor){
			campaign->setCursor(cursorPoint.x,cursorPoint.y);
		}
	}
	return false;
}

void Layer_BattleField::whenPressConfirm(){
	auto scene=dynamic_cast<Scene_BattleField*>(parentObject);
	if(!scene)return;
	//确定操作
	if(isEditMode){
		auto p=campaign->cursor;
		if(scene->menuMapEdit.selectingItemIndex==Scene_BattleField::MapEdit_Delete){//删除单位
			battleField->removeUnit(p.x,p.y);
		}else{//添加地形或单位
			if(isEditMode_Unit){//添加单位
				battleField->addUnit(p.x,p.y,scene->menuCorpSelect.selectingItemIndex,scene->menuCorpSelect.troopID);
			}else{//设置地形
				Terrain terrain(scene->menuTerrainSelect.selectingItemIndex,scene->menuTerrainSelect.troopID);
				battleField->setTerrain(p.x,p.y,terrain);
				battleField->autoAdjustTerrainTile(p.x,p.y,true);
			}
		}
	}
}
void Layer_BattleField::whenPressCancel(){}

void Layer_BattleField::renderX()const{
	renderTerrains();//画地形图块
	renderMovements();//渲染移动范围
	renderFireRange();//渲染火力范围
	renderMovePath();//渲染路径
	renderGrid();//网格绘制
	renderUnits();//画单位
	renderCursor();//画光标
}

void Layer_BattleField::updateMapRect(){
	//更新size
	size.setXY(battleField->getWidth()*latticeSize,battleField->getHeight()*latticeSize);
	//更新mapRect变量
	auto rct=rectF();
	mapRect.p0.setXY(rct.p0.x,rct.p0.y);
	mapRect.p1.setXY(rct.p1.x,rct.p1.y);
}

void Layer_BattleField::renderTerrains()const{
	GAME_AW
	int w=battleField->getWidth(),h=battleField->getHeight();
	Terrain terrain;
	for(decltype(h) y=h-1;y>=0;--y){//一定要自上而下渲染
		for(decltype(w) x=0;x<w;++x){
			//取地形,并取纹理
			battleField->getTerrain(x,y,terrain);
			auto tex=game->terrainsTexturesArray.getTexture(terrain.terrainType,terrain.status);
			updateRenderPos(x,y);
			tex.draw(renderPos);
		}
	}
}

void Layer_BattleField::renderUnits()const{
	for(auto &cp:battleField->chessPieces){
		spriteUnit.setUnit(cp);
		updateRenderPos(cp.coordinate.x,cp.coordinate.y);
		spriteUnit.position.setXY(renderPos.x,renderPos.y);
		spriteUnit.render();
	}
}
void Layer_BattleField::renderMovements()const{
	for(auto &p:campaign->movablePoints){
		updateRenderLattice(p.x,p.y);
		ShapeRenderer::drawRectangle(renderLattice,nullptr,&rgbMove);
	}
}
void Layer_BattleField::renderFireRange()const{
	for(auto &p:campaign->firablePoints){
		updateRenderLattice(p.x,p.y);
		ShapeRenderer::drawRectangle(renderLattice,nullptr,&rgbFire);
	}
}
void Layer_BattleField::renderMovePath()const{
	for(auto &p:campaign->movePath){
		updateRenderLattice(p.x,p.y);
		ShapeRenderer::drawRectangle(renderLattice,nullptr,&rgbMovePath);
	}
}
void Layer_BattleField::renderGrid()const{
	//开始渲染每一个方格
	auto w=battleField->getWidth(),h=battleField->getHeight();
	for(decltype(h) y=0;y<h;++y){
		for(decltype(w) x=0;x<w;++x){
			//计算grid的参数,并渲染
			updateRenderLattice(x,y);
			ShapeRenderer::drawRectangle(renderLattice,&ColorRGBA::White,nullptr);
		}
	}
}
void Layer_BattleField::renderCursor()const{
	auto &cursor=campaign->cursor;
	updateRenderLattice(cursor.x,cursor.y);
	texCursor.draw(renderPos);
}