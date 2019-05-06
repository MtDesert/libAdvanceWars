#ifndef UNIT_H
#define UNIT_H

#include"ChessPiece.h"
#include"Point.h"
#include"List.h"
#include"typedef.h"

//<高级战争>战场上的一个单位
struct Unit:public ChessPiece<uint8,uint8>{
	//构造函数
	Unit(uint8 corpType=0,
		 uint8 troopType=0,
		 const decltype(coordinate) &p=decltype(coordinate)(0,0),
		 uint8 healthPower=100,
		 uint8 fuel=99,
		 uint8 ammunition=0);
	//判断本单位和Unit是否一致
	bool operator==(const Unit &unit)const;

	//表现HP,实际HP的另外的表现形式,正常范围(0~10)
	static uint8 presentHP(uint8 hp);
	uint8 presentHP()const;
	//成员变量
	uint8 healthPower;//HP值(正常范围0~100)
	uint8 fuel;//燃料值
	uint8 ammunition;//弹药数量(有时表示材料数量)
	uint8 progressValue;//进度值(占领或者建造时候用,正常范围0~20)
	bool isVisible:1;//可见性,图形引擎扫描到此值为true时,应该进行绘制
	bool isWait:1;//是否待机状态,待机状态下一般不可移动
	bool isHide:1;//是否隐藏状态(特定兵种可用此状态)
	uchar level:2;//单位等级(取值0~3)
	//自己所搭载的单位
	List<Unit> loadedUnits;
};

class UnitList:public List<Unit>{
public:
	Unit* findUnit_byCoordinate(int x,int y);
};

#endif
