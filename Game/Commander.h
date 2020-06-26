#ifndef COMMANDER_H
#define COMMANDER_H

#include"DataList.h"
#include"Number.h"

struct CommanderPowerFeature{
	CommanderPowerFeature();
	//条件
	string corpType;//兵种类型(默认为所有兵种全符合条件)
	string terrainType;//地形类型(默认为所有处于terrainType的单位全符合条件)
	string weatherType;//天气类型(默认为所有在weatherType下的单位全符合条件)
	//状态效果-据点
	int capturableIncome;//据点收入
	int capturableRepairHP;//据点修复量
	//状态效果-单位
	IntRange damageFix;//损伤修正的最大最小值(百分数),默认为{0,10}
	int attack;//攻击修正百分数
	int counterAttack;//反击修正百分数(累乘attack)
	int defence;//防御百分比
	int directDefence;//防御直接攻击百分数
	int indirectDefence;//防御间接攻击百分数
	int enemyTerrainDefendLVminus;//敌方地形防御等级减少
	int movement;//移动力
	int attackRangeMax;//最大射程
	int enemyDamageTransformSelfFunds;//敌人损伤转化自身资金率(百分数)
	int fuelConsumePerDay;//日消耗油量
	//瞬发效果
	string executeFunction;//执行瞬间发动的技能
	//运算符号
	CommanderPowerFeature& operator+=(const CommanderPowerFeature &another);
};

//指挥官能力值
struct CommanderPower{
	CommanderPower();
	string name,translate;//名字,翻译
	int energySlot;//能量槽
	Array<CommanderPowerFeature> allFeatures;//特性描述
};

/*指挥官(CO=Commander)数据,仅仅用来查看*/
struct Commander{
	string name,troop;//名字,所属部队
	string note,quote;//指挥官的说明,口头禅
	string coHit,coMiss;//喜欢和头疼的事物(不知怎么翻译)
	string d2d;//描述CO部队的日常能力
	Array<CommanderPower> allPowers;//所有能力,包括日常能力和发动后的能力
	int commandRange;//指挥范围
	string author;//设计此CO的作者
};

class CommandersList:public DataList<Commander>{
public:
	bool loadFile_lua(const string &filename,WhenErrorString whenError=nullptr);//加载lua格式的数据文件,返回错误信息(返回nullptr表示无错误)
};

#endif// COMMANDER_H