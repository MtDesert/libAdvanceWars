#include"Corp.h"
#include"LuaState.h"

Weapon::Weapon():minRange(1),maxRange(1),ammunitionMax(0){}
Corp::Corp():price(0),vision(0),movement(0),gasMax(0),
capturable(false),suppliable(false),hidable(false),repairable(false),explodable(false),buildable(false),flarable(false){}

#define READ_BOOL(name) corp->name=state.getTableBoolean(#name);
#define READ_INT(name) corp->name=state.getTableInteger(#name);
#define READ_STR(name) state.getTableString(#name,corp->name);

bool CorpsList::loadFile_lua(const string &filename,WhenErrorString whenError){
	bool ret=false;
	LuaState state;
	state.whenError=whenError;
	if(state.doFile(filename)){
		ret=state.getGlobalTable("Corps",[this,&state](){
			setSize(state.getTableLength(),true);//设置表长
			return state.getTableForEach([this,&state](int index){
				auto corp=this->data(index);
				if(!corp)return false;
				//读取数据
				READ_STR(name)
				READ_STR(corpType)
				READ_INT(price)
				READ_INT(vision)
				state.getTableTable("move",[&state,corp](){
					READ_INT(movement)
					READ_STR(moveType)
					READ_INT(gasMax)
					return true;
				});
				return true;
			});
		});
	}
	return ret;
}