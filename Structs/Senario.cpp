#include "Senario.h"

//#include<lua.hpp>
#include<iostream>
#include<unistd.h>

Senario* Senario::senario=NULL;

Senario::Senario():isFogWar(false),showingMenu(false),menuChanged(false),state_Senario(NULL)
{
	speakPosY=0;
	speakSkip=true;
}
void Senario::initLuaState()
{
	//reopen
	//if(state_Senario)lua_close(state_Senario);
	//state_Senario=lua_newstate();
	//load C++ code for support
	//lua(state_Senario);
	//luaopen_AdvanceWars(state_Senario);
	//registerLuaFunction_Senario(state_Senario);
	//lua_register(state_Senario,"print",print);
	//lua_pcall(state_Senario,0,LUA_MULTRET,0);
}

void Senario::loadLuaScript_Rule(const string &filename)
{
	//lua_pushcfunction(state_Senario,&errfunc);
	//luaL_loadfile(state_Senario,filename.data());
	//lua_pcall(state_Senario,0,LUA_MULTRET,1);
}
int Senario::print(lua_State *state)
{
	//cout<<lua_tostring(state,1)<<endl;
	cout.flush();
	return 0;
}
int Senario::errfunc(lua_State *state){print(state);return 0;}

void Senario::start_senarioStart()
{
	pthread_create(&thread,NULL,run_senarioStart,NULL);
}
void Senario::start_afterUnitMove()
{
	pthread_create(&thread,NULL,run_afterUnitMoved,NULL);
}

void Senario::touch(int x, int y)
{
	//lua_pushcfunction(state_Senario,&errfunc);
	//lua_getglobal(state_Senario,"cursorTouch");
	//lua_pushnumber(state_Senario,x);
	//lua_pushnumber(state_Senario,y);
	//lua_pcall(state_Senario,2,0,1);
}
void Senario::menuTouch(int index)
{
	//lua_pushcfunction(state_Senario,&errfunc);
	//lua_getglobal(state_Senario,"menuTouch");
	//lua_pushnumber(state_Senario,index);
	//lua_pcall(state_Senario,1,0,1);
}
void Senario::doString(const string &command)
{
	//lua_pushcfunction(state_Senario,&errfunc);
	//luaL_loadstring(state_Senario,command.data());
	//lua_pcall(state_Senario,0,LUA_MULTRET,1);
}

#define SENARIO_LUA_REGISTER(name) lua_register(state,#name,name);
void Senario::registerLuaFunction_Senario(lua_State *state)
{
	//SENARIO_LUA_REGISTER(showSay)
	//SENARIO_LUA_REGISTER(say)
	//SENARIO_LUA_REGISTER(hideSay)
	//
}
//call by lua
int Senario::showSay(lua_State *state)
{
	while(senario->speakPosY<100)
	{
		++senario->speakPosY;
		usleep(5000);
	}
	return 0;
}
int Senario::say(lua_State *state)
{
	//senario->speakerHeadImage=lua_tostring(state,1);
	//senario->speakerName=lua_tostring(state,2);
	//senario->speakerContent=lua_tostring(state,3);
	//wait speakSkip
	//senario->speakSkip=false;
	//while(!senario->speakSkip)usleep(1);
	return 0;
}
int Senario::hideSay(lua_State *state)
{
	while(senario->speakPosY>0)
	{
		--senario->speakPosY;
		usleep(5000);
	}
	return 0;
}

pthread_t Senario::thread;
void* Senario::run_senarioStart(void*)
{
	//lua_getglobal(senario->state_Senario,"senarioStart");
	//lua_call(senario->state_Senario,0,0);
	return 0;
}
void* Senario::run_afterUnitMoved(void*)
{
	//lua_getglobal(senario->state_Senario,"afterUnitMoved");
	//lua_call(senario->state_Senario,0,0);
	return 0;
}
