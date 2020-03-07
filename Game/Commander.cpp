#include"Commander.h"
#include"LuaState.h"

#define COMMANDER_ATTR(luaname,cppname,type)\
if(str==#luaname){co.cppname=lua_to##type(state,-1);}else

#define READ(obj,member,type) state.getTable##type(#member,obj->member);
#define READ_CORP_BOOL(member) corp->member=state.getTableBoolean(#member);

bool CommandersList::loadFile_lua(const string &filename,WhenErrorString whenError){
	bool ret=false;
	LuaState state;
	state.whenError=whenError;
	if(state.doFile(filename)){
		ret=state.getGlobalTable("Commanders",[&](){
			setSize(state.getTableLength(),true);//设置表长
			return state.getTableForEach([&](int index){
				auto co=this->data(index);
				if(!co)return false;
				//读取数据
				READ(co,name,String)
				READ(co,note,String)
				READ(co,quote,String)
				READ(co,coHit,String)
				READ(co,coMiss,String)
				return true;
			});
		});
	}
	return ret;
}