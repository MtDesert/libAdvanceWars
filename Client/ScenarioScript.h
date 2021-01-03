#ifndef SCENARIOSCRIPT_H
#define SCENARIOSCRIPT_H

#include"GameScript.h"

//所有的高级战争用的剧情脚本,根据需求添加更多脚本
#define SCENARIOSCRIPT_ALL_FUNCTIONS(MACRO) \
MACRO(showBodyImage)/*显示身体形象*/\
MACRO(standAt)/*身体形象站在特定位置*/\
MACRO(faceTo)/*身体形象看向某方向或某处*/\
MACRO(hide)/*身体形象消失*/

//高级战争-剧情脚本,用于有序执行脚本
class ScenarioScript:public GameScript{
public:
	ScenarioScript();
	~ScenarioScript();

	virtual void scriptInit();
	//脚本函数
#define FUNCTION_DECL(name) static int name(lua_State *state);
	SCENARIOSCRIPT_ALL_FUNCTIONS(FUNCTION_DECL)
#undef FUNCTION_DECL
};
#endif