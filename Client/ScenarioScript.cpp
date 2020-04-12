#include"ScenarioScript.h"
#include"Game_AdvanceWars.h"

static Game_AdvanceWars *game=nullptr;
static LuaState *staticLuaScript=nullptr;
//注册成员函数
#define MEMBER_FUNC(name) staticLuaScript->addClassFunction(#name,name);
//获取对象信息
#define GET_SELF(className) auto self=reinterpret_cast<className*>(staticLuaScript->getTableUserData("self"));

//个人全身
class PersonBody:public GameSprite{
public:
	//c++
	PersonBody(const char *name){//生成身体形象
		texture.texImage2D_FilePNG(game->settings.bodyImagePath+"/"+name+".png",Game::whenError);
		setTexture(texture);
	}
	void show(){
		auto layer=game->useLayerConversation();
		layer->addSubObject(this);
		game->addSubObject(layer);
	}
	//lua
	static int show(lua_State *state){
		GET_SELF(PersonBody)
		self->show();
		return 0;
	}
	static int PersonBody_new(lua_State *state){
		auto name=lua_tostring(state,-1);
		staticLuaScript->addClassObjectSelf(new PersonBody(name));
		MEMBER_FUNC(show)
		return 1;
	}
};

ScenarioScript::ScenarioScript(){}
ScenarioScript::~ScenarioScript(){staticLuaScript=nullptr;}

//脚本函数
void ScenarioScript::scriptInit(){
	GameScript::scriptInit();
	game=Game_AdvanceWars::currentGame();
	staticLuaScript=this->luaScript;
	luaScript->registerClass("Body",PersonBody::PersonBody_new);
}