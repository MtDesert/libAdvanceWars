#include "Dialog_NewMap.h"

Dialog_NewMap::Dialog_NewMap(){
	const SizeType itemWidth=400,spacing=16;
	//初始化标题
	stringMapInfo.setString("MapInfo",true);
	addSubObject(&stringMapInfo);

#define LABEL_INIT(Name,Type) \
	attr##Name.setLabel_Spacing_MaxWidth(#Name,true,spacing,itemWidth);\
	addSubObject(&attr##Name);

	LABEL_INIT(MapName,String)
	LABEL_INIT(Author,String)
	LABEL_INIT(Width,Integer)
	LABEL_INIT(Height,Integer)

	addSubObject(&buttonsConfirmCancel);

	//调整尺寸
	size.x = itemWidth + spacing*2;
	allSubObjects_verticalLayout(spacing);
	//设置取值范围

	//默认事件
	buttonsConfirmCancel.buttonCancel.onClicked=[&](){removeFromParentObject();};
}

void Dialog_NewMap::setBattleField(const BattleField &battleField){
	attrMapName.inputBox.setString(battleField.mapName);
	attrAuthor.inputBox.setString(battleField.author);
	attrWidth.inputBox.setInteger(battleField.getWidth());
	attrHeight.inputBox.setInteger(battleField.getHeight());
}
void Dialog_NewMap::resetBattleField(BattleField &battleField)const{
	battleField.mapName=attrMapName.getValue();
	battleField.author=attrAuthor.getValue();
	battleField.newData(attrWidth.getValue(),attrHeight.getValue());
}
void Dialog_NewMap::setConfirmCallback(GameButton::ClickCallback onConfirm){
	buttonsConfirmCancel.buttonConfirm.onClicked=onConfirm;
}