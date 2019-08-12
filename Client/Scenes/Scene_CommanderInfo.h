#ifndef SCENE_COMMANDERINFO_H
#define SCENE_COMMANDERINFO_H

#include"Commander.h"
#include"GameScene.h"
#include"GameText.h"
#include"Texture.h"

class Scene_CommanderInfo:public GameScene{
public:
	Scene_CommanderInfo();
	~Scene_CommanderInfo();

	CommandersList *source;//数据源,用来显示信息
	TextureCache *bodyTextures;//CO的全身像,用来显示对应的CO信息
	void setCoInfo(uint index);//设置要显示的信息

	virtual void keyboardKey(Keyboard::KeyboardKey key,bool pressed);
protected:
	uint coIndex;//co的索引号

	GameString stringName,strName;//姓名
	GameString stringQuote,strQuote;//口头禅
	GameString stringNote;//说明
	GameText txtNote;
	GameString stringD2D;//日常
	GameText txtD2D;
	GameSprite spriteBody;//全身像
};

#endif