#ifndef LAYER_BATTLEFIELD_H
#define LAYER_BATTLEFIELD_H

#include"GameLayer.h"
#include"Campaign.h"

//高级战争的战场图层,战场可能很大,需要查看特定的区域时候需要移动本类对象
class Layer_BattleField:public GameLayer{
public:
	//构造/析构函数
	Layer_BattleField();
	~Layer_BattleField();

	//地图数据
	BattleField *battleField;//这是战场,主要存地形和单位
	Campaign *campaign;//这是战役,战役相关的数据都在这里
	Campaign::CoordType cursorPoint;//光标所处的坐标
	//操作模式
	bool isEditMode:1;//是否处于编辑模式
	bool isEditMode_Unit:1;//false时候为地形编辑模式,true的时候为单位编辑模式

	//输入事件
	virtual bool keyboardKey(Keyboard::KeyboardKey key,bool pressed);//处理光标移动
	virtual bool mouseKey(MouseKey key,bool pressed);//处理触摸效果
	virtual bool mouseMove(int x,int y);//处理光标跟随鼠标效果

	void setCursor(const Campaign::CoordType &p);//设置光标位置
	void whenPressConfirm();//确定操作
	void whenPressCancel();//取消操作
	//渲染函数
	virtual void renderX()const;//画战场
	//更新函数
	void updateMapRect();//更新地图矩形,影响渲染
protected:
	void renderTerrains()const;//渲染地形
	void renderFog()const;//渲染雾效果
	void renderUnits()const;//渲染单位
	void renderMovements()const;//渲染移动范围
	void renderFireRange()const;//渲染火力范围
	void renderMovePath()const;//渲染移动路径
	void renderGrid()const;//渲染网格
	void renderCursor()const;//渲染光标
};
#endif