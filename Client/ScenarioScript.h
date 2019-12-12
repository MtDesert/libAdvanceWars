#ifndef SCENARIOSCRIPT_H
#define SCENARIOSCRIPT_H

#include"GameScript.h"

//高级战争-剧情脚本,用于有序执行脚本
class ScenarioScript:public GameScript{
public:
	ScenarioScript();
	~ScenarioScript();

	virtual void scriptInit();
};
#endif