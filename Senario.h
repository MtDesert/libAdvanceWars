#ifndef SENARIO_H
#define SENARIO_H

#include"BattleField.h"
#include"Commander.h"
#include"Unit.h"

#include"lua.hpp"
#include<pthread.h>
#include<string>
using namespace std;

int luaopen_AdvanceWars(lua_State* tolua_S);

class Senario
{
public:
	Senario();
	void initLuaState();

	void loadLuaScript_Rule(const string &filename);
	static int print(lua_State*);
	static int errfunc(lua_State*);

	static Senario *senario;

	//for move
	struct MovePoint:public Point2D<uint>
	{
		MovePoint(const Point2D<uint> &p=Point2D<uint>()):Point2D<uint>(p){}
		uchar remainMovement,remainFuel;
	};
	bool isFogWar;
	list<MovePoint> movablePoints;
	list<MovePoint> movePath;
	list<Point2D<uint> > visiblePoints;
	list<Point2D<uint> > firablePoints;
	list<Point2D<uint> > dropablePoints;

	//game menu
	bool showingMenu;
	list<string> menuItems;
	bool menuChanged;

	//conversation
	int speakPosY;
	string speakerHeadImage;//maybe a filename
	string speakerName;
	string speakerContent;
	bool speakSkip;

	//pass to lua
	void touch(int x,int y);
	void menuTouch(int index);
	void doString(const string &command);

	void start_senarioStart();
	void start_afterUnitMove();
private:
	static void registerLuaFunction_Senario(lua_State*);
#define SENARIO_LUA_FUNCTION(name) static int name(lua_State*)
	//rule function
	//senario function
	SENARIO_LUA_FUNCTION(showSay);
	SENARIO_LUA_FUNCTION(say);
	SENARIO_LUA_FUNCTION(hideSay);
	//C language function
private:
	lua_State *state_Senario;

	static pthread_t thread;
	static void* run_senarioStart(void*);
	static void* run_afterUnitMoved(void*);
};

#endif // SENARIO_H
