#include"Commander.h"
#include"LuaState.h"

#define READ(obj,member,type) state.getTable##type(#member,obj->member);
#define READ_STR(member) READ(feature,member,String)
#define READ_INT(member) READ(feature,member,Integer)
#define READ_BOOL(member) READ(feature,member,Boolean)

CommanderPowerFeature::CommanderPowerFeature():capturableIncome(0),capturableRepairHP(0),damageFix(0,10),
attack(0),counterAttack(0),defence(0),directDefence(0),indirectDefence(0),enemyTerrainDefendLVminus(0),
movement(0),captureSpeed(0),attackRangeMax(0),enemyDamageTransformSelfFunds(0),fuelConsumePerDay(0){}

CommanderPowerFeature& CommanderPowerFeature::operator+=(const CommanderPowerFeature &another){
	damageFix=another.damageFix;
#define ACCUMULATE(name) name += another.name;
			ACCUMULATE(attack)//攻击
			ACCUMULATE(counterAttack)//反击
			ACCUMULATE(defence)//防御
			ACCUMULATE(directDefence)//直接防御
			ACCUMULATE(indirectDefence)//间接防御
			ACCUMULATE(enemyTerrainDefendLVminus)//敌方地形防御减少
			ACCUMULATE(movement)//移动力
			ACCUMULATE(captureSpeed)//占领速度
			ACCUMULATE(attackRangeMax)//最大射程
			ACCUMULATE(enemyDamageTransformSelfFunds)//敌方损伤转化资金率
#undef ACCUMULATE
	return *this;
}

CommanderPower::CommanderPower():energySlot(0){}

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
				//读取各个CO数据
				READ(co,name,String)
				READ(co,troop,String)
				READ(co,note,String)
				READ(co,quote,String)
				READ(co,coHit,String)
				READ(co,coMiss,String)
				//读取能力表
				return state.getTableTable("powers",[&](){
					co->allPowers.setSize(state.getTableLength(),true);
					return state.getTableForEach([&](int idx){
						auto power=co->allPowers.data(idx);
						if(!power)return false;
						//读取power的数据
						READ(power,name,String)
						READ(power,translate,String)
						READ(power,energySlot,Integer)
						state.getTableTable("features",[&](){
							power->allFeatures.setSize(state.getTableLength(),true);
							return state.getTableForEach([&](int i){
								auto feature=power->allFeatures.data(i);
								if(!feature)return false;
								//读取feature的过滤条件
								READ_STR(corpType)
								READ_STR(terrainType)
								READ_STR(weatherType)
								//读取feature的状态效果
								READ_INT(capturableIncome)
								READ_INT(capturableRepairHP)
								//IntRange damageFix)损伤修正的最大最小值(百分数),默认为{0,10}
								READ_INT(attack)
								READ_INT(counterAttack)
								READ_INT(defence)
								READ_INT(directDefence)
								READ_INT(indirectDefence)
								READ_INT(enemyTerrainDefendLVminus)
								READ_INT(movement)
								READ_INT(captureSpeed)
								READ_INT(attackRangeMax)
								READ_INT(enemyDamageTransformSelfFunds)
								READ_INT(fuelConsumePerDay)
								state.getTableTable("damageFix",[&](){
									return state.getTableForEach([&](int m){
										if(m==0)state.getTopInteger(feature->damageFix.minimun);
										if(m==1)state.getTopInteger(feature->damageFix.maximun);
										return true;
									});
								});
								READ_STR(executeFunction)
								return true;
							});
						});
						return true;
					});
				});
			});
		});
	}
	return ret;
}