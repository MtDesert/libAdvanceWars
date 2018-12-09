#include"Corp.h"
#include"LuaState.h"

Weapon::Weapon():minRange(1),maxRange(1),ammunitionMax(0){}
Corp::Corp():id(0),price(0),vision(0),movement(0),gasMax(0),
capturable(false),suppliable(false),hidable(false),repairable(false),explodable(false),buildable(false),flarable(false){}
Corp::~Corp(){}

string CorpsList::loadFile_lua(const string &filename){
	lua_State *state=luaL_newstate();
	//加载文件
	//搜索兵种表
	lua_getglobal(state,"Corps");
	if(lua_gettop(state)!=1)return("No table \'Corps\'");
	if(!lua_istable(state,1))return("\'Corps\' not table");
	//遍历兵种表
	int corpID=0;
	lua_pushnil(state);//遍历前要push nil
	while(lua_next(state,-2)){//-2是key的位置
		if(lua_istable(state,-1)){//-1是value的位置
			Corp corp;//读取用的缓冲
			corp.id=corpID;
			lua_pushnil(state);
			while(lua_next(state,-2)){
				string str=lua_tostring(state,-2);
				if(str=="name"){//兵种名称
					corp.name=lua_tostring(state,-1);
				}else if(str=="corpType"){//兵种类型
					corp.corpType=lua_tostring(state,-1);
				}else if(str=="price"){//造价
					corp.price=lua_tointeger(state,-1);
				}else if(str=="vision"){//视野
					corp.vision=lua_tointeger(state,-1);
				}else if(str=="move" && lua_istable(state,-1)){
					lua_pushnil(state);
					while(lua_next(state,-2)){
						string str0=lua_tostring(state,-2);
						if(str0=="movement"){//移动力
							corp.movement=lua_tointeger(state,-1);
						}else if(str0=="moveType"){//移动类型
							corp.moveType=lua_tostring(state,-1);
						}else if(str0=="gasMax"){//燃料
							corp.gasMax=lua_tointeger(state,-1);
						}
						lua_pop(state,1);
					}
				}else if(str=="weapons" && lua_istable(state,-1)){
					lua_pushnil(state);
					while(lua_next(state,-2) && lua_istable(state,-1)){//武器表
						Weapon weapon;
						lua_pushnil(state);
						while(lua_next(state,-2)){
							string str0=lua_tostring(state,-2);
							if(str0=="name"){//武器名
								weapon.name=lua_tostring(state,-1);
							}else if(str0=="minRange"){//武器最小射程
								weapon.minRange=lua_tointeger(state,-1);
							}else if(str0=="maxRange"){//武器最大射程
								weapon.maxRange=lua_tointeger(state,-1);
							}else if(str0=="ammunitionMax"){//武器弹药
								weapon.ammunitionMax=lua_tointeger(state,-1);
							}
							lua_pop(state,1);
						}
						corp.weapons.push_back(weapon);
						lua_pop(state,1);
					}
				}else if(str=="capturable"){
					corp.capturable=lua_toboolean(state,-1);
				}else if(str=="suppliable"){
					corp.suppliable=lua_toboolean(state,-1);
				}else if(str=="hidable"){
					corp.hidable=lua_toboolean(state,-1);
				}else if(str=="repairable"){
					corp.repairable=lua_toboolean(state,-1);
				}else if(str=="explodable"){
					corp.explodable=lua_toboolean(state,-1);
				}else if(str=="buildable"){
					corp.buildable=lua_toboolean(state,-1);
				}else if(str=="flarable"){
					corp.flarable=lua_toboolean(state,-1);
				}
				lua_pop(state,1);
			}
			push_back(corp);
			++corpID;
		}
		lua_pop(state,1);
	}
	//结束
	return "";
}
