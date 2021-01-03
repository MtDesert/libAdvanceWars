#ifndef LAYER_CONVERSATION_H
#define LAYER_CONVERSATION_H

#include"GameLayer_Conversation.h"

/*对话界面,在大多数游戏中,对话可以出现在各种场合,比如特定的剧情对话,教程引导等*/
class Layer_Conversation:public GameLayer_Conversation{
public:
	Layer_Conversation();
	~Layer_Conversation();
protected:
	virtual Texture* bodyImage_str2tex(const char *name)const;
};
#endif