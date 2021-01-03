#include"ScenarioScript.h"
#include"Game_AdvanceWars.h"

static Game_AdvanceWars *game=nullptr;
static LuaState *staticLuaScript=nullptr;

ScenarioScript::ScenarioScript(){}
ScenarioScript::~ScenarioScript(){staticLuaScript=nullptr;}

void ScenarioScript::scriptInit(){
	GameScript::scriptInit();
	game=Game_AdvanceWars::currentGame();
	staticLuaScript=this->luaScript;
	//注册枚举
#define CASE(name) luaScript->setGlobalInteger(#name,BodyImage::name);
	CASE(Left)
	CASE(MiddleLeft)
	CASE(Middle)
	CASE(MiddleRight)
	CASE(Right)
#undef CASE
	//注册函数
#define CASE(name) luaScript->registerFunction(#name,name);
	SCENARIOSCRIPT_ALL_FUNCTIONS(CASE)
#undef CASE
}

//注册对象的成员函数
#define FUNC(name) ls.addClassFunction(#name,name);
//获取ClassName类型的对象
#define GET_SELF(ClassName)\
LuaState ls(state);\
auto self=reinterpret_cast<ClassName*>(ls.getClassObjectSelf(1));
//只返回self对象本身给lua
#define RETURN_SELF \
ls.clearStack(1);\
return 1;

//剧情脚本函数
int ScenarioScript::showBodyImage(lua_State *state){
	//获取参数
	auto name=lua_tostring(state,1);
	//显示对象
	auto layer=game->useLayerConversation();
	auto bodyImage=layer->showBodyImage(name);
	//返回值:bodyImage对象
	LuaState ls(state);
	ls.addClassObjectSelf(bodyImage);
	FUNC(standAt)
	FUNC(faceTo)
	FUNC(hide)
	return 1;
}
int ScenarioScript::standAt(lua_State *state){
	//获取参数
	GET_SELF(BodyImage)
	//移动位置
	int pos;
	if(ls.getInteger(2,pos)){
		self->standAt(pos);
	}
	//返回值
	RETURN_SELF
}
int ScenarioScript::faceTo(lua_State *state){
	//获取参数
	GET_SELF(BodyImage)
	//移动位置
	int dir;
	if(ls.getInteger(2,dir)){
		self->faceTo(dir);
	}
	//返回值
	RETURN_SELF
}
int ScenarioScript::hide(lua_State *state){
	GET_SELF(BodyImage)
	self->hide();
	RETURN_SELF
}