#ifndef SENARIO_H
#define SENARIO_H

#include"BattleField.h"
#include"Commander.h"
#include"Unit.h"

#include<lua.h>
#include<String.h>
#include<pthread.h>

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

	//battle map & units
	BattleField battleField;
	UnitList unitList;
	//for move
	struct MovePoint:public Point
	{
		MovePoint(const Point &p=Point()):Point(p){}
		uchar remainMovement,remainFuel;
	};
	bool isFogWar;
	List<MovePoint> movablePoints;
	List<MovePoint> movePath;
	List<Point> visiblePoints;
	List<Point> firablePoints;
	List<Point> dropablePoints;

	//game menu
	bool showingMenu;
	List<string> menuItems;
	bool menuChanged;

	//conversation
	int speakPosY;
	String speakerHeadImage;//maybe a filename
	String speakerName;
	String speakerContent;
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