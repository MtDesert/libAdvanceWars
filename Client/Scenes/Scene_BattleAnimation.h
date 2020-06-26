#ifndef SCENE_BATTLEANIMATION_H
#define SCENE_BATTLEANIMATION_H

#include"GameScene.h"
#include"GameString.h"
#include"Campaign.h"

//显示单位相关信息的区域
struct Scene_BattleAnimation_UnitInfo:public GameSprite{
	Scene_BattleAnimation_UnitInfo();

	GameString stringAttack,stringDefence,stringDamage;//攻击,防御,损伤
	GameString strAttack,strDefence,strDamage;//攻击,防御,损伤

	void reset();
	void refreshLayout();
	void setDamageData(const DamageData &data);
};

//战斗场景的半边画面
struct Scene_BattleAnimation_Half:public GameSprite{//战斗画面-半边
	Scene_BattleAnimation_Half();

	GameSprite background;//背景,用于显示
	GameSprite unitSprite[10];//单位的精灵,用于播放动画
	GameSprite headImage;//头像,显示指挥官是谁
	Scene_BattleAnimation_UnitInfo unitInfoPanel;//单位信息,显示单位的详细数据

	virtual void reset();
	void refreshLayout(bool isRight);//刷新布局
	void setDamageData(const DamageData &damageData);
};

/*战斗动画场景,负责播放动画*/
class Scene_BattleAnimation:public GameScene{
	Scene_BattleAnimation_Half leftPart,rightPart;//战斗画面的左右部分
public:
	Scene_BattleAnimation();
	~Scene_BattleAnimation();

	virtual bool keyboardKey(Keyboard::KeyboardKey key,bool pressed);//debug
	void playAnimation(const Campaign &campaign);//播放动画
};

#endif//SCENE_BATTLEANIMATION_H