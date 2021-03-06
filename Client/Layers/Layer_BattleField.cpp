#include"Layer_BattleField.h"
#include"Sprite_Terrain.h"
#include"Sprite_Unit.h"
#include"Game_AdvanceWars.h"

#define BATTLEFIELD_SCENE auto scene=dynamic_cast<Scene_BattleField*>(parentObject);

static int latticeSize=32;//格子大小
//渲染变量
static Rectangle2D<int> mapRect;//地图矩形
static Point2D<float> renderPos;//渲染位置
static Rectangle2D<float> renderLattice;//渲染格子

static Sprite_Terrain spriteTerrain;//地形精灵,用于绘制地形
static Sprite_Unit spriteUnit;//单位精灵,用于渲染单位
static GameSprite spriteCursor;//光标,用于指示操作位置
static GameSprite latticeMove,latticeFire,latticeMovePath,latticeFog,latticeImpactRange;//移动范围,开火范围,移动路径范围,雾范围(影响视野范围),波及范围

static void updateRenderPos(int x,int y){//更新渲染位置,根据x,y来更新renderPos
	renderPos.setXY(mapRect.p0.x + x*latticeSize,mapRect.p0.y + y*latticeSize);
}
static void updateRenderLattice(int x,int y){//更新需要渲染的网格数据
	updateRenderPos(x,y);
	renderLattice.p0=renderPos;
	renderLattice.p1.setXY(renderLattice.p0.x +latticeSize-1,renderLattice.p0.y +latticeSize-1);
}

Layer_BattleField::Layer_BattleField():battleField(nullptr),campaign(nullptr),isEditMode(false),isEditMode_Unit(false),
animationUnit(nullptr),whenAnimationUnitMoveOver(nullptr){
	forceIntercept=true;
	GAME_AW
	battleField=&game->battleField;//读取数据用
	campaign=&game->campaign;
	//精灵初始化
	spriteTerrain.anchorPoint.setXY(0,0);
	spriteUnit.anchorPoint.setXY(0,0);
	spriteCursor.anchorPoint.setXY(0,0);
	spriteTerrain.terrainsTexArray=&game->terrainsTexturesArray;//地形渲染
	spriteUnit.unitTexArray=&game->corpsIconsArray;//渲染单位用
	spriteUnit.numTexArray=&game->numbersTextures;//渲染HP用
	spriteCursor.setTexture(game->allIconsTextures.getTexture("Cursor"));
	//范围设定,这里要区分一下GDI和OpenGL的格式
#ifdef __MINGW32__
#define TO_FORMAT toBGRA
#define LATTICE_W_H latticeSize
#else
#define TO_FORMAT toRGBA
#define LATTICE_W_H 1
#endif

#define MAKE_LATTICE_TEXTURE(lattice,color) \
	lattice.setTexture(Texture::makeSolidTexture(LATTICE_W_H,LATTICE_W_H,ColorRGBA::color.TO_FORMAT()));\
	lattice.setColor(ColorRGBA(0x80FFFFFF));\
	lattice.size.setXY(latticeSize,latticeSize);\
	lattice.anchorPoint.setXY(0,0);

	MAKE_LATTICE_TEXTURE(latticeMove,Blue)
	MAKE_LATTICE_TEXTURE(latticeFire,Red)
	MAKE_LATTICE_TEXTURE(latticeMovePath,Green)
	MAKE_LATTICE_TEXTURE(latticeFog,Black)
	MAKE_LATTICE_TEXTURE(latticeImpactRange,Yellow)
	//动画相关
	game->timeSliceList.pushTimeSlice(this,20,40);
	campaign->whenExecuteMoveUnit=[&](const Campaign::CoordType &oldPos,const Unit &unit){//单位移动动画
		BATTLEFIELD_SCENE
		scene->allowInput(false);//播放动画时禁止触摸
		playMoveUnit(unit,oldPos);//单位移动
	};
}
Layer_BattleField::~Layer_BattleField(){
	GAME_AW
	game->timeSliceList.removeTimeSlice(this);
	latticeMove.texture.deleteTexture();
	latticeFire.texture.deleteTexture();
	latticeMovePath.texture.deleteTexture();
}

bool Layer_BattleField::keyboardKey(Keyboard::KeyboardKey key,bool pressed){
	bool ret=false;
	if(!pressed){
		ret=true;
		if(campaign->selectedTargetPoint && !campaign->selectedTargetPointFreely){//从目标列表中选择
			switch(key){
				case Keyboard::Key_Up:case Keyboard::Key_Left:campaign->choosePrevTarget();break;
				case Keyboard::Key_Down:case Keyboard::Key_Right:campaign->chooseNextTarget();break;
				case Keyboard::Key_Enter:whenPressConfirm();break;
				case Keyboard::Key_Esc:whenPressCancel();break;
				default:ret=false;
			}
		}else{
			int x=0,y=0;
			switch(key){
				case Keyboard::Key_Up:y=1;break;
				case Keyboard::Key_Down:y=-1;break;
				case Keyboard::Key_Left:x=-1;break;
				case Keyboard::Key_Right:x=1;break;
				case Keyboard::Key_Enter:whenPressConfirm();break;
				case Keyboard::Key_Esc:whenPressCancel();break;
				case Keyboard::Key_F1:
					if(campaign->selectedUnitData.unit && campaign->cursorUnitData.unit){//进入战斗界面
						GAME_AW
						campaign->damageCaculator->attackPredict();//预测战斗数据
						auto scene=game->gotoScene_BattleAnimation(true);
						scene->playAnimation(*campaign);
					}
				break;
				default:ret=false;
			}
			if(x || y){//方向键,进行移动
				setCursor(campaign->cursor + Campaign::CoordType(x,y));
			}
		}
	}
	return ret;
}

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
	//计算出鼠标对应的地图格子坐标
	GAME_AW
	auto pos=game->mousePos-(mapRect.p0 + position);
	cursorPoint.setXY(pos.x/latticeSize,pos.y/latticeSize);
	//判断是否需要更新
	if(cursorPoint != campaign->cursor){
		setCursor(cursorPoint);
	}
	return false;
}

void Layer_BattleField::setCursor(const Campaign::CoordType &p){
	BATTLEFIELD_SCENE
	campaign->setCursor(p);
	scene->setCursor(p);
}
void Layer_BattleField::whenPressConfirm(){
	BATTLEFIELD_SCENE
	//编辑模式
	if(isEditMode){
		auto p=campaign->cursor;
		if(scene->menuMapEdit->selectingItemIndex==Scene_BattleField::MapEdit_Delete){//删除单位
			battleField->removeUnit(p.x,p.y);
		}else{//添加地形或单位
			if(isEditMode_Unit){//添加单位
				battleField->addUnit(p.x,p.y,scene->menuCorpSelect->selectingItemIndex,scene->menuCorpSelect->troopID);
			}else{//设置地形
				Terrain terrain(scene->menuTerrainSelect->selectingItemIndex,scene->menuTerrainSelect->troopID);
				battleField->setTerrain(p.x,p.y,terrain);
				battleField->autoAdjustTerrainTile(p.x,p.y,true);
			}
		}
	}else{//打仗模式
		campaign->cursorConfirm();
		scene->updateMenu();
	}
}
void Layer_BattleField::whenPressCancel(){
	BATTLEFIELD_SCENE
	//取消选择
	campaign->cursorCancel();
	scene->updateMenu();
}
void Layer_BattleField::playMoveUnit(const Unit &unit,const BattleField::CoordType &oldPos){
	//计算动画数据
	auto delta=oldPos-unit.coordinate;
	animationUnit=&unit;
	animationUnitOffset.setXY(delta.x * latticeSize,delta.y * latticeSize);
}

#define UNIT_MOVE_SPEED 4
void Layer_BattleField::consumeTimeSlice(){
	if(!animationUnit)return;
	//x变换
	if(animationUnitOffset.x>0)animationUnitOffset.x-=UNIT_MOVE_SPEED;
	if(animationUnitOffset.x<0)animationUnitOffset.x+=UNIT_MOVE_SPEED;
	//y变换
	if(animationUnitOffset.y>0)animationUnitOffset.y-=UNIT_MOVE_SPEED;
	if(animationUnitOffset.y<0)animationUnitOffset.y+=UNIT_MOVE_SPEED;
	//结束
	if(animationUnitOffset.x==0 && animationUnitOffset.y==0){//一格移动完成
		animationUnit=nullptr;
		if(whenAnimationUnitMoveOver)whenAnimationUnitMoveOver();
	}
}
void Layer_BattleField::renderX()const{
	renderTerrains();//画地形图块
	renderFog();//渲染雾
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

//自上而下渲染
#define RENDER_UP_TO_DOWN(code) \
int w=battleField->getWidth(),h=battleField->getHeight();\
for(decltype(h) y=h-1;y>=0;--y){\
	for(decltype(w) x=0;x<w;++x){\
		code\
	}\
}


void Layer_BattleField::renderTerrains()const{
	Terrain terrain;
	RENDER_UP_TO_DOWN(
		//取地形,并取纹理
		battleField->getTerrain(x,y,terrain);
		spriteTerrain.setTerrain(terrain);
		updateRenderPos(x,y);
		spriteTerrain.position.setXY(renderPos.x,renderPos.y);
		spriteTerrain.render();
	)
}

void Layer_BattleField::renderFog()const{
	Terrain terrain;
	RENDER_UP_TO_DOWN(
		//取地形,并取纹理
		battleField->getTerrain(x,y,terrain);
		if(!terrain.isVisible){//看不见的地形要隐藏
			updateRenderLattice(x,y);
			latticeFog.position.setXY(renderPos.x,renderPos.y);
			latticeFog.render();
		}
	)
}

void Layer_BattleField::renderUnits()const{
	Unit *unit=nullptr;
	RENDER_UP_TO_DOWN(
		unit=battleField->getUnit(x,y);
		if(unit && unit->isVisible){
			spriteUnit.setUnit(*unit);
			updateRenderPos(unit->coordinate.x,unit->coordinate.y);
			spriteUnit.position.setXY(renderPos.x,renderPos.y);
			if(animationUnit==unit){//动画偏移修正
				spriteUnit.position = spriteUnit.position + animationUnitOffset;
			}
			spriteUnit.render();
		}
	)
}

#define RENDER_LATTICE(lattice) \
updateRenderLattice(p.x,p.y);\
lattice.position.setXY(renderPos.x,renderPos.y);\
lattice.render();

void Layer_BattleField::renderMovements()const{
	for(auto &p:campaign->movablePoints){
		RENDER_LATTICE(latticeMove)
	}
}
void Layer_BattleField::renderFireRange()const{
	for(auto &p:campaign->firablePoints){
		RENDER_LATTICE(latticeFire)
	}
}
void Layer_BattleField::renderMovePath()const{
	for(auto &p:campaign->movePath){
		RENDER_LATTICE(latticeMovePath)
	}
}
void Layer_BattleField::renderGrid()const{
	//开始渲染每一个方格
	auto w=battleField->getWidth(),h=battleField->getHeight();
	for(decltype(h) y=0;y<h;++y){
		for(decltype(w) x=0;x<w;++x){
			//计算grid的参数,并渲染
			updateRenderLattice(x,y);
			//ShapeRenderer::drawRectangle(renderLattice,&ColorRGBA::White,nullptr);
		}
	}
}
void Layer_BattleField::renderCursor()const{
	auto &cursor=campaign->cursor;
	updateRenderPos(cursor.x,cursor.y);
	spriteCursor.position.setXY(renderPos.x,renderPos.y);
	spriteCursor.render();
}
