#include"Scene_BattleAnimation.h"
#include"ShapeRenderer.h"

struct Scene_BattleAnimation_Info{//战斗动画,信息框
};

struct Scene_BattleAnimation_Half:public GameSprite{//战斗画面-半边
	GameSprite background;//背景,用于显示
	GameSprite unitSprite[10];//单位的精灵,用于播放动画
	GameSprite headImage;//头像,显示指挥官是谁
};

static Scene_BattleAnimation_Half leftPart,rightPart;

Scene_BattleAnimation::Scene_BattleAnimation(){
	addSubObject(&leftPart);
	addSubObject(&rightPart);
}
Scene_BattleAnimation::~Scene_BattleAnimation(){}

void Scene_BattleAnimation::renderX()const{}