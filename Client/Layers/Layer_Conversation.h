#ifndef LAYER_CONVERSATION_H
#define LAYER_CONVERSATION_H

#include"GameLayer.h"
#include"GameText.h"

/*对话界面,在大多数游戏中,对话可以出现在各种场合,比如特定的剧情对话,教程引导等*/
class Layer_Conversation:public GameLayer{
public:
	Layer_Conversation();
	~Layer_Conversation();

	//成员
	GameSprite dialogBox;//对话框
	GameText dialogText;//对话框的文本
	bool pausePrintText;//是否暂停显示对话

	virtual void keyboardKey(Keyboard::KeyboardKey key,bool pressed);
	virtual void consumeTimeSlice();//根据时间片,对文字进行逐个显示

	void setDialogText(const string &str);
};
#endif