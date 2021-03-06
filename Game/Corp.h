#ifndef CORP_H
#define CORP_H

#include"DataList.h"

struct Weapon{
	Weapon();

	string name;//武器名
	uint8 minRange,maxRange;//射程
	uint8 ammunitionMax;//武器弹药数

	uint8 flareRange;//闪光弹闪光范围

	bool isDirectAttack()const;
	bool isIndirectAttack()const;
};
/*兵种数据*/
struct Corp{
	Corp();

	string name;//兵种名字
	string translate;//翻译
	string corpType;//兵种类型
	uint16 price;//造价
	uint8 vision;//视野
	//移动相关
	uint8 movement;//移动力
	string moveType;//移动类型
	uint8 gasMax;//燃料,耗尽时候无法移动
	//特性相关
	bool capturable:1;//是否具有占领能力
	bool suppliable:1;//是否具有补给能力
	bool hidable:1;//是否具有隐藏能力
	bool repairable:1;//是否具有修复能力
	bool explodable:1;//是否具有爆炸能力
	bool buildable:1;//是否具有建造能力
	//武器相关
	Array<Weapon> weapons;
	Weapon* firstAttackableWeapon()const;//首个可攻击武器
	Weapon* firstFlarableWeapon()const;//首个可闪光武器

	bool isDirectAttack()const;//是否为直接攻击的兵种
	bool isIndirectAttack()const;//是否为间接攻击的兵种

	//lua交互
	string toLuaString()const;//根据当前数据输出成lua的格式
};

/*兵种数据列表,提供加载文件的功能*/
class CorpsList:public DataList<Corp>{
public:
	bool loadFile_lua(const string &filename,WhenErrorString whenError);//加载lua格式的数据文件,返回错误信息(返回nullptr表示无错误)
};
#endif