#ifndef SCENE_BATTLEANIMATION_H
#define SCENE_BATTLEANIMATION_H

#include"GameScene.h"

/*战斗动画场景,负责播放动画*/
class Scene_BattleAnimation:public GameScene{
public:
	Scene_BattleAnimation();
	~Scene_BattleAnimation();

	//override
	virtual void renderX()const;
};

#endif//SCENE_BATTLEANIMATION_H