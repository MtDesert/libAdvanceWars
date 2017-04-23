#ifndef GAMESCENE_MAIN_H
#define GAMESCENE_MAIN_H

#include"GameScene.h"
#include"List.h"
#include<string>

//主场景,显示一下商标
class GameScene_Main:public GameScene{
public:
	GameScene_Main();
	
	void render()const;
protected:
	void consumeTimeSlice();
private:
	int status;
	int countDown;//倒计时
	int contentBrightness;//内容亮度
	//
	List<string> menuItemNames;
};
#endif