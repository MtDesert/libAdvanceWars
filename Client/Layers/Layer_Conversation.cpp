#include"Layer_Conversation.h"
#include"Game_AdvanceWars.h"

static Game_AdvanceWars *game=nullptr;
Layer_Conversation::Layer_Conversation(){
	game=Game_AdvanceWars::currentGame();
}
Layer_Conversation::~Layer_Conversation(){}

Texture* Layer_Conversation::bodyImage_str2tex(const char *name)const{
	auto idx=game->mCommandersList.indexOf([&](const Commander &cmder){
		return cmder.name==name;
	});
	return game->commandersBodyTextures.data(idx);
}