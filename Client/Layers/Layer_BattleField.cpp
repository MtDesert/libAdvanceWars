#include"Layer_BattleField.h"
#include"Sprite_Terrain.h"
#include"Sprite_Unit.h"
#include"Game_AdvanceWars.h"

static int latticeSize=32;//格子大小
//渲染变量
static Rectangle2D<int> mapRect;//地图矩形
static Point2D<float> renderPos;//渲染位置
static Rectangle2D<float> renderLattice;//渲染格子

static Sprite_Terrain spriteTerrain;//地形精灵,用于绘制地形
static Sprite_Unit spriteUnit;//单位精灵,用于渲染单位
static GameSprite spriteCursor;//光标,用于指示操作位置
static GameSprite latticeMove,latticeFire,latticeMovePath;

static void updateRenderPos(int x,int y){//更新渲染位置,根据x,y来更新renderPos
	renderPos.setXY(mapRect.p0.x + x*latticeSize,mapRect.p0.y + y*latticeSize);
}
static void updateRenderLattice(int x,int y){//更新需要渲染的网格数据
	updateRenderPos(x,y);
	renderLattice.p0=renderPos;
	renderLattice.p1.setXY(renderLattice.p0.x +latticeSize-1,renderLattice.p0.y +latticeSize-1);
}

Layer_BattleField::Layer_BattleField():battleField(nullptr),campaign(nullptr),
isEditMode(false),isEditMode_Unit(false){
	forceIntercept=true;
	GAME_AW
	battleField=&game->battleField;//读取数据用
	campaign=&game->campaign;
	//精灵初始化
	spriteTerrain.anchorPoint.setXY(0,0);
	spriteUnit.anchorPoint.setXY(0,0);
	spriteCursor.anchorPoint.setXY(0,0);
	spriteUnit.unitTexArray=&game->corpsTexturesArray;//渲染单位用
	spriteUnit.numTexArray=&game->numbersTextures;//渲染HP用
	Texture texCursor;
	texCursor.texImage2D_FilePNG(game->settings.imagesPathIcons+"/Cursor.png",Game::whenError);
	spriteCursor.setTexture(texCursor);
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
}
Layer_BattleField::~Layer_BattleField(){
	latticeMove.texture.deleteTexture();
	latticeFire.texture.deleteTexture();
	latticeMovePath.texture.deleteTexture();
}

bool Layer_BattleField::keyboardKey(Keyboard::KeyboardKey key,bool pressed){
	bool ret=false;
	if(!pressed){
		ret=true;
		if(campaign->selectedTargetPoint){//从目标列表中选择
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
				case Keyboard::Key_F1:campaign->endTurn();break;
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

#define BATTLEFIELD_SCENE auto scene=dynamic_cast<Scene_BattleField*>(parentObject);

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
			spriteTerrain.setTexture(tex);
			updateRenderPos(x,y);
			spriteTerrain.position.setXY(renderPos.x,renderPos.y);
			spriteTerrain.render();
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
			ShapeRenderer::drawRectangle(renderLattice,&ColorRGBA::White,nullptr);
		}
	}
}
void Layer_BattleField::renderCursor()const{
	auto &cursor=campaign->cursor;
	updateRenderPos(cursor.x,cursor.y);
	spriteCursor.position.setXY(renderPos.x,renderPos.y);
	spriteCursor.render();
}