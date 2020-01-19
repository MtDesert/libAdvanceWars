#include"Scene_BattleField.h"
#include"Game_AdvanceWars.h"

#include"extern.h"

static int latticeSize=32;//格子大小
static GameSprite trnSprite;//地形渲染用的精灵

Scene_BattleField::Scene_BattleField():battleField(nullptr),terrainsTexturesArray(nullptr),corpsTextures(nullptr){
	trnSprite.anchorPoint.y=0;
}
Scene_BattleField::~Scene_BattleField(){}

bool Scene_BattleField::keyboardKey(Keyboard::KeyboardKey key,bool pressed){
	bool ret=false;
	bool isDirectionKey=
		(key==Keyboard::Key_Up)||
		(key==Keyboard::Key_Down)||
		(key==Keyboard::Key_Left)||
		(key==Keyboard::Key_Right);
	if(isDirectionKey && !pressed){
		ret=true;
	}else if(key==Keyboard::Key_Enter && !pressed){
		cursorSelect();//处理选择事件
		ret=true;
	}else if(key==Keyboard::Key_F1 && !pressed){
		auto sz=sizeF();
		printf("%d,%d\n",(int)sz.x,(int)sz.y);
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
			cursorSelect();
		}
		//改变状态
		mouseKeyDown=pressed;
		isTouchMove=false;
	}
	return false;
}
bool Scene_BattleField::mouseMove(int x,int y){
	if(mouseKeyDown){//有可能是拖动
		if(isTouchMove){//移动地图
			//改变位置
			position.x-=(x-touchMovePoint.x);
			position.y-=(y-touchMovePoint.y);
		}else{//判断移动距离是不是很大,是的话切换到移动模式
			if((touchBeginPoint-Game::mousePos).distance2()>=latticeSize*latticeSize){
				isTouchMove=true;
			}
		}
		touchMovePoint.x=x;
		touchMovePoint.y=y;
	}else{//鼠标移动
		auto res=Game::resolution;
		auto fieldLen=sizeF();
		if(fieldLen.x<=res.x){//宽度没超出分辨率
			x-=(res.x-fieldLen.x)/2;
		}else{
			x+=position.x;
		}
		if(fieldLen.y<=res.y){//高度没超出分辨率
			y-=(res.y-fieldLen.y)/2;
		}else{
			y+=position.y;
		}
	}
	return false;
}

void Scene_BattleField::renderX()const{
	renderTerrains();
	renderGrid();//网格绘制
	//画地形图块
	//画移动范围
	//画移动路径
	//画攻击范围
	//画单位
	//画光标
	//看情况显示兵种命令
}
Point2D<float> Scene_BattleField::sizeF()const{
	point2D.setXY(
		battleField ? battleField->getWidth()*latticeSize : 0,
		battleField ? battleField->getHeight()*latticeSize : 0);
	return point2D;
}

void Scene_BattleField::renderTerrains()const{
	rect=rectF();
	auto w=battleField->getWidth(),h=battleField->getHeight();
	Terrain terrain;
	Point2D<float> p;
	for(decltype(h) y=0;y<h;++y){
		for(decltype(w) x=0;x<w;++x){
			//取地形,并取纹理
			battleField->getTerrain(x,y,terrain);
			auto arr=terrainsTexturesArray->data(terrain.terrainType);
			if(arr){
				auto tex=arr->data(terrain.status);
				if(tex){
					p.setXY(rect.p0.x + x*latticeSize,rect.p0.y + y*latticeSize);
					tex->draw(p);
				}
			}
		}
	}
}
void Scene_BattleField::renderGrid()const{
	rect=rectF();
	Rectangle2D<float> grid;
	//开始渲染每一个方格
	auto w=battleField->getWidth(),h=battleField->getHeight();
	for(decltype(h) y=0;y<h;++y){
		for(decltype(w) x=0;x<w;++x){
			//计算grid的参数,并渲染
			grid.p0.setXY(rect.p0.x + x*latticeSize,rect.p0.y + y*latticeSize);
			grid.p1.setXY(grid.p0.x +latticeSize-1,grid.p0.y +latticeSize-1);
			shapeRenderer.drawRectangle(grid,&ColorRGBA::White,nullptr);
		}
	}
}
void Scene_BattleField::cursorSelect(){}