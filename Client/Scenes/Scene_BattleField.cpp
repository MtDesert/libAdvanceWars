#include"Scene_BattleField.h"
#include"Sprite_Unit.h"
#include"Menu_CorpCommand.h"
#include"Game_AdvanceWars.h"

#include"extern.h"

static int latticeSize=32;//格子大小
//渲染变量
static Game_AdvanceWars *game=nullptr;
static Rectangle2D<int> mapRect;//地图矩形
static Point2D<float> renderPos;//渲染位置
static Rectangle2D<float> renderLattice;//渲染格子

static Sprite_Unit spriteUnit;//单位精灵,用于渲染单位
static Texture texCursor;//光标纹理,用于渲染光标

static Menu_CorpCommand menuCorpCommand;//兵种命令菜单,用于显示命令
static ColorRGBA rgbMove,rgbFire,rgbMovePath;//移动区域的颜色,可攻击区域的颜色,移动路径颜色

static void updateRenderPos(int x,int y){//更新渲染位置,根据x,y来更新renderPos
	renderPos.setXY(mapRect.p0.x + x*latticeSize,mapRect.p0.y + y*latticeSize);
}
static void updateRenderLattice(int x,int y){//更新需要渲染的网格数据
	updateRenderPos(x,y);
	renderLattice.p0=renderPos;
	renderLattice.p1.setXY(renderLattice.p0.x +latticeSize-1,renderLattice.p0.y +latticeSize-1);
}

Scene_BattleField::Scene_BattleField():battleField(nullptr){
	game=Game_AdvanceWars::currentGame();
	battleField=&game->battleField;//读取数据用
	campaign=&game->campaign;
	//精灵初始化
	spriteUnit.anchorPoint.setXY(0,0);
	spriteUnit.texArray=&game->corpsTexturesArray;//渲染单位用
	texCursor.texImage2D_FilePNG("images/Icons/Cursor.png",Game::whenError);
	//颜色设定
	rgbMove.blue=255;rgbMove.alpha=128;
	rgbFire.red=255;rgbFire.alpha=128;
	rgbMovePath.green=255;rgbMovePath.alpha=128;
	//菜单
	menuCorpCommand.bgColor=&ColorRGBA::Black;
	menuCorpCommand.borderColor=&ColorRGBA::White;
	menuCorpCommand.corpMenu=&campaign->corpMenu;
}
Scene_BattleField::~Scene_BattleField(){}

bool Scene_BattleField::keyboardKey(Keyboard::KeyboardKey key,bool pressed){
	bool ret=false;
	if(!pressed){
		ret=true;
		int x=0,y=0;
		switch(key){
			case Keyboard::Key_Up:y=1;break;
			case Keyboard::Key_Down:y=-1;break;
			case Keyboard::Key_Left:x=-1;break;
			case Keyboard::Key_Right:x=1;break;
			case Keyboard::Key_Enter:
				campaign->cursorSelect();
				updateMenu();
			break;
			case Keyboard::Key_Esc:
				campaign->cursorCancel();
			break;
			default:ret=false;
		}
		//移动方向
		if(x || y){
			campaign->setCursor(campaign->cursor.x+x,campaign->cursor.y+y);
		}
	}
	return ret;
}

//拖动效果变量
static decltype(Game::mousePos) touchBeginPoint,touchMovePoint;//触摸起点,移动中的点
static bool mouseKeyDown=false;//是否按下鼠标
static bool isTouchMove=false;//是否拖动行为

bool Scene_BattleField::mouseKey(MouseKey key,bool pressed){
	if(key==Mouse_LeftButton){
		if(pressed){//点击时候记录位置
			touchBeginPoint = Game::mousePos;
			touchMovePoint = Game::mousePos;
		}else if(isTouchMove);//如果是拖动,放手后一般不处理
		else{//处理点击事件
			campaign->cursorSelect();
		}
		//改变状态
		mouseKeyDown=pressed;
		isTouchMove=false;
	}else if(key==Mouse_RightButton && !pressed){
		campaign->cursorCancel();
	}
	return false;
}
bool Scene_BattleField::mouseMove(int x,int y){
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
		auto pos=game->mousePos-(mapRect.p0 + position);
		cursorPoint.setXY(pos.x/latticeSize,pos.y/latticeSize);
		//判断是否需要更新
		if(cursorPoint!=campaign->cursor){
			campaign->setCursor(cursorPoint.x,cursorPoint.y);
		}
	}
	return false;
}

void Scene_BattleField::renderX()const{
	renderTerrains();//画地形图块
	renderMovements();//渲染移动范围
	renderFireRange();//渲染火力范围
	renderMovePath();//渲染路径
	renderGrid();//网格绘制
	renderUnits();//画单位
	renderCursor();//画光标
}
Point2D<float> Scene_BattleField::sizeF()const{
	point2D.setXY(
		battleField ? battleField->getWidth()*latticeSize : 0,
		battleField ? battleField->getHeight()*latticeSize : 0);
	return point2D;
}
void Scene_BattleField::updateMapRect(){
	auto rct=rectF();
	mapRect.p0.setXY(rct.p0.x,rct.p0.y);
	mapRect.p1.setXY(rct.p1.x,rct.p1.y);
}
void Scene_BattleField::updateMenu(){
	if(!menuCorpCommand.parentObject && campaign->corpMenu.size()){
		//动态生成兵种命令菜单
		addSubObject(&menuCorpCommand);
		menuCorpCommand.updateData();
	}
	if(menuCorpCommand.parentObject && campaign->corpMenu.size()==0){
		removeSubObject(&menuCorpCommand);
	}
}

void Scene_BattleField::renderTerrains()const{
	int w=battleField->getWidth(),h=battleField->getHeight();
	Terrain terrain;
	for(decltype(h) y=h-1;y>=0;--y){//一定要自上而下渲染
		for(decltype(w) x=0;x<w;++x){
			//取地形,并取纹理
			battleField->getTerrain(x,y,terrain);
			auto arr=game->terrainsTexturesArray.data(terrain.terrainType);
			if(arr){
				auto tex=arr->data(terrain.status);
				if(tex){
					updateRenderPos(x,y);
					tex->draw(renderPos);
				}
			}
		}
	}
}

void Scene_BattleField::renderUnits()const{
	for(auto &cp:battleField->chessPieces){
		spriteUnit.setUnit(cp);
		updateRenderPos(cp.coordinate.x,cp.coordinate.y);
		spriteUnit.position.setXY(renderPos.x,renderPos.y);
		spriteUnit.render();
	}
}
void Scene_BattleField::renderMovements()const{
	for(auto &p:campaign->movablePoints){
		updateRenderLattice(p.x,p.y);
		shapeRenderer.drawRectangle(renderLattice,nullptr,&rgbMove);
	}
}
void Scene_BattleField::renderFireRange()const{
	for(auto &p:campaign->firablePoints){
		updateRenderLattice(p.x,p.y);
		shapeRenderer.drawRectangle(renderLattice,nullptr,&rgbFire);
	}
}
void Scene_BattleField::renderMovePath()const{
	for(auto &p:campaign->movePath){
		updateRenderLattice(p.x,p.y);
		shapeRenderer.drawRectangle(renderLattice,nullptr,&rgbMovePath);
	}
}
void Scene_BattleField::renderGrid()const{
	//开始渲染每一个方格
	auto w=battleField->getWidth(),h=battleField->getHeight();
	for(decltype(h) y=0;y<h;++y){
		for(decltype(w) x=0;x<w;++x){
			//计算grid的参数,并渲染
			updateRenderLattice(x,y);
			shapeRenderer.drawRectangle(renderLattice,&ColorRGBA::White,nullptr);
		}
	}
}
void Scene_BattleField::renderCursor()const{
	auto &cursor=campaign->cursor;
	updateRenderLattice(cursor.x,cursor.y);
	texCursor.draw(renderPos);
}