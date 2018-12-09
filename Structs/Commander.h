#ifndef COMMANDER_H
#define COMMANDER_H

#include"DataList.h"

/*指挥官(CO=Commander)数据,仅仅用来查看*/
struct Commander{
	string coName,note,quote;//名字,指挥官的说明,口头禅
	string coHit,coMiss;//喜欢和头疼的事物(不知怎么翻译)
	string d2d;//描述CO部队的日常能力
	struct Power{//CO可发动的能力
		string name,note;//能力名称,能力造成的效果
		int starCount;//能量星数,决定了累积多少能量才能发动技能
	};
	Power power,superPower;//一般来说CO都有普通能力和超能力,也可能只有一种或没有
	int commandRange;//指挥范围
	string author;//设计此CO的作者
};

class CommandersList:public DataList<Commander>{
public:
	string loadFile_lua(const string &filename);//加载lua格式的数据文件,返回错误信息(返回nullptr表示无错误)
};

#endif // COMMANDER_H
