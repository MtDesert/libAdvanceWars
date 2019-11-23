#include"Layer_Conversation.h"
#include"Game.h"

Layer_Conversation::Layer_Conversation():pausePrintText(false){
	//对话框,出现在屏幕底部
	dialogBox.anchorPoint.y=0;
	dialogBox.position.y=-Game::resolution.y/2;
	dialogBox.size.setXY(Game::resolution.x,dialogText.charSize.y*3);
	dialogBox.borderColor=&ColorRGBA::White;
	addSubObject(&dialogBox);
	//对话文本,在对话框中显示文本内容
	dialogText.position.setXY(-dialogBox.size.x/2,dialogBox.size.y);
	dialogText.anchorPoint.setXY(0,1);
	dialogText.setLineCharAmount(3,dialogBox.size.x/dialogText.charSize.x);
	dialogBox.addSubObject(&dialogText);
	//添加时间片让对话框根据时间来运动
	Game::currentGame()->timeSliceList.pushTimeSlice(this,100,100);
}
Layer_Conversation::~Layer_Conversation(){
	Game::currentGame()->timeSliceList.removeTimeSlice(this);
}

void Layer_Conversation::keyboardKey(Keyboard::KeyboardKey key,bool pressed){
	if(key==Keyboard::Key_Enter && !pressed){
		if(pausePrintText){
			pausePrintText=false;
			dialogText.printNextLine();
			//dialogText.printNextChar();
		}
	}
}
void Layer_Conversation::consumeTimeSlice(){
	if(pausePrintText)return;
	pausePrintText=dialogText.printNextChar();//逐个显示字符
}
void Layer_Conversation::setDialogText(const string &str){
	dialogText.setString(str);
	dialogText.readyPrint();
}