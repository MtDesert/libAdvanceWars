#include"Corp.h"
#include"LuaState.h"

Weapon::Weapon():minRange(1),maxRange(1),ammunitionMax(0),flareRange(0){}
Corp::Corp():price(0),vision(1),movement(0),gasMax(0),
capturable(false),suppliable(false),hidable(false),repairable(false),explodable(false),buildable(false){}

bool Weapon::isDirectAttack()const{return minRange==1 && maxRange==1;}
bool Weapon::isIndirectAttack()const{return maxRange>=2;}

Weapon* Corp::firstAttackableWeapon()const{return weapons.data([](const Weapon &wpn){return !wpn.flareRange;});}
Weapon* Corp::firstFlarableWeapon()const{return weapons.data([](const Weapon &wpn){return wpn.flareRange;});}

bool Corp::isDirectAttack()const{
	auto wpn=firstAttackableWeapon();
	return wpn ? wpn->isDirectAttack():false;
}
bool Corp::isIndirectAttack()const{
	auto wpn=firstAttackableWeapon();
	return wpn ? wpn->isIndirectAttack():false;
}

#include"Number.h"

#define WRITE_STR(name) ret+=#name"=\""+name+"\",";
#define WRITE_INT(name) ret+=#name"="+Number::toString(name)+",";
#define WRITE_BOOL(name) if(name)ret+=#name"=true,";
#define WRITE_STR_WPN(name) ret+=#name"=\""+wpn.name+"\",";
#define WRITE_INT_WPN(name) ret+=#name"="+Number::toString(wpn.name)+",";

string Corp::toLuaString()const{
	string ret("{");//输出基本属性
	WRITE_STR(name)
	WRITE_STR(translate)
	WRITE_STR(corpType)
	WRITE_INT(price)
	WRITE_INT(vision)
	ret+="move={";//输出移动相关数据
	WRITE_INT(movement)
	WRITE_STR(moveType)
	WRITE_INT(gasMax)
	ret.pop_back();ret+="},";
	if(weapons.size()){//输出武器数据
		ret+="weapons={";
		weapons.forEach([&](const Weapon &wpn){
			ret+="{";
			WRITE_STR_WPN(name)
			if(wpn.minRange!=1)WRITE_INT_WPN(minRange);
			if(wpn.maxRange!=1)WRITE_INT_WPN(maxRange);
			if(wpn.ammunitionMax)WRITE_INT_WPN(ammunitionMax)
			if(wpn.flareRange)WRITE_INT_WPN(flareRange)
			ret+="},";
		});
		ret+="},";
	}
	//
	WRITE_BOOL(capturable)
	WRITE_BOOL(suppliable)
	WRITE_BOOL(hidable)
	WRITE_BOOL(repairable)
	WRITE_BOOL(explodable)
	WRITE_BOOL(buildable)
	ret+="}";
	return ret;
}

#define READ_STR(obj,member) state.getTableString(#member,obj->member);
#define READ_INT(obj,member) obj->member=state.getTableInteger(#member);
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
				READ_STR(corp,name)
				READ_STR(corp,translate)
				READ_STR(corp,corpType)
				READ_INT(corp,price)
				READ_INT(corp,vision)
				state.getTableTable("move",[&](){//读取移动信息
					READ_INT(corp,movement)
					READ_STR(corp,moveType)
					READ_INT(corp,gasMax)
					return true;
				});
				state.getTableTable("weapons",[&](){//读取武器列表
					corp->weapons.setSize(state.getTableLength(),true);
					state.getTableForEach([&](int idx){
						auto wpn=corp->weapons.data(idx);
						if(!wpn)return false;
						//读取武器信息
						READ_STR(wpn,name)
						READ_INT(wpn,minRange)
						READ_INT(wpn,maxRange)
						READ_INT(wpn,ammunitionMax)
						READ_INT(wpn,flareRange)
						//修正射程
						if(wpn->minRange<=0)wpn->minRange=1;
						if(wpn->maxRange<=0)wpn->maxRange=1;
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
				return true;
			});
		});
	}
	return ret;
}