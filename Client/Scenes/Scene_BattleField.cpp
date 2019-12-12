#include"Scene_BattleField.h"
#include"Game_AdvanceWars.h"
#include"GameCamera.h"

#include"extern.h"

static int latticeSize=32;//格子大小

Scene_BattleField::Scene_BattleField():terrainsTextures(nullptr),corpsTextures(nullptr){}
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
	//网格绘制设定
	//画地形图块
	//画移动范围
	//画移动路径
	//画攻击范围
	//画单位
	//画光标
	//看情况显示兵种命令
}
Point2D<float> Scene_BattleField::sizeF()const{
	if(battleField){
		point2D.setXY(battleField->getWidth()*latticeSize,battleField->getHeight()*latticeSize);
	}else{
		point2D.setXY(0,0);
	}
	return point2D;
}

void Scene_BattleField::cursorSelect(){}