#include"Corp.h"
#include"LuaState.h"

Weapon::Weapon():minRange(1),maxRange(1),ammunitionMax(0){}
Corp::Corp():price(0),vision(0),movement(0),gasMax(0),
capturable(false),suppliable(false),hidable(false),repairable(false),explodable(false),buildable(false),flarable(false){}

bool Weapon::isDirectAttack()const{return minRange==1 && maxRange==1;}
bool Weapon::isIndirectAttack()const{return maxRange>=2;}

bool Corp::isDirectAttack()const{
	auto wpn=weapons.data(0);
	return wpn ? wpn->isDirectAttack():false;
}
bool Corp::isIndirectAttack()const{
	auto wpn=weapons.data(0);
	return wpn ? wpn->isIndirectAttack():false;
}

#define READ(obj,member,type) state.getTable##type(#member,obj->member);
#define READ_CORP_BOOL(member) corp->member=state.getTableBoolean(#member);

bool CorpsList::loadFile_lua(const string &filename,WhenErrorString whenError){
	bool ret=false;
	LuaState state;
	state.whenError=whenError;
	if(state.doFile(filename)){
		ret=state.getGlobalTable("Corps",[this,&state](){
			setSize(state.getTableLength(),true);//设置表长
			return state.getTableForEach([&](int index){
				auto corp=this->data(index);
				if(!corp)return false;
				//读取数据
				READ(corp,name,String)
				READ(corp,translate,String)
				READ(corp,corpType,String)
				READ(corp,price,Integer)
				READ(corp,vision,Integer)
				state.getTableTable("move",[&](){//读取移动信息
					READ(corp,movement,Integer)
					READ(corp,moveType,String)
					READ(corp,gasMax,Integer)
					return true;
				});
				state.getTableTable("weapons",[&](){//读取武器列表
					corp->weapons.setSize(state.getTableLength(),true);
					state.getTableForEach([&](int idx){
						auto wpn=corp->weapons.data(idx);
						if(!wpn)return false;
						//读取武器信息
						READ(wpn,name,String)
						READ(wpn,minRange,Integer)
						READ(wpn,maxRange,Integer)
						READ(wpn,ammunitionMax,Integer)
						return true;
					});
					return true;
				});
				//读取兵种特性参数
				READ_CORP_BOOL(capturable)
				READ_CORP_BOOL(suppliable)
				READ_CORP_BOOL(hidable)
				READ_CORP_BOOL(repairable)
				READ_CORP_BOOL(explodable)
				READ_CORP_BOOL(buildable)
				READ_CORP_BOOL(flarable)
				return true;
			});
		});
	}
	return ret;
}