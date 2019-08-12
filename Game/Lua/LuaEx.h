#ifndef LUAEX_H
#define LUAEX_H

#include"lua.hpp"
#include<string>
using namespace std;

string loadfile_pcall(lua_State *state,const string &filename);

#endif
