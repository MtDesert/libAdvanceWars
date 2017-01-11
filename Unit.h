#ifndef UNIT_H
#define UNIT_H

#include"ChessPiece.h"
#include"List.h"
#include"String.h"
#include"typedef.h"

struct CorpCode{};
struct TroopCode{
	String name;
	uint colors[4];
};
class TroopCodeList:public List<TroopCode>{};

class Unit:public ChessPiece
{
public:
	typedef ChessPiece::CorpType CorpType;
	typedef ChessPiece::Color ColorType;

	Unit(CorpType corpType=0,
		 ColorType troopType=0,
		 const Point &coordinate=Point(0,0),
		 uint8 healthPower=100,
		 uint8 fuel=99,
		 uint8 ammunition=9);
	bool operator==(const Unit &unit)const;

	static uint8 presentHP(uint8 hp);
	uint8 presentHP()const;
	//attrib
	uint8 healthPower;
	uint8 fuel;
	uint8 ammunition;//or material
	uint8 progressValue;//capture or build
	bool isVisible:1;
	bool isWait:1;
	bool isHide:1;
	uchar level:2;
	//
	List<Unit> loadedUnits;
};

class UnitList:public List<Unit>
{
public:
	Unit* findUnit_byCoordinate(short x,short y);
};

#endif // UNIT_H