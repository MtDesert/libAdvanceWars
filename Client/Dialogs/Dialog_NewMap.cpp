#include "Dialog_NewMap.h"

Dialog_NewMap::Dialog_NewMap(){
	const SizeType itemWidth=400,spacing=16;
	//初始化标题
	stringMapInfo.setString("MapInfo",true);
	addSubObject(&stringMapInfo);

#define LABEL_INIT(Name,Type) \
	attr##Name.setLabelName_ValueWidth_MaxWidth(#Name,true,280,itemWidth);\
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
	attrMapName.inputBox.setValue(battleField.mapName);
	attrAuthor.inputBox.setValue(battleField.author);
	attrWidth.inputBox.setValue(battleField.getWidth());
	attrHeight.inputBox.setValue(battleField.getHeight());
}
void Dialog_NewMap::resetBattleField(BattleField &battleField)const{
	battleField.mapName=attrMapName.getValue();
	battleField.author=attrAuthor.getValue();
	auto w=attrWidth.getUnsignedValue(),h=attrHeight.getUnsignedValue();
	if(w!=battleField.getWidth() || h!=battleField.getHeight()){//尺寸不一致,需要重新调整
		battleField.newData(w,h);
	}
}
void Dialog_NewMap::setConfirmCallback(GameButton::ClickCallback onConfirm){
	buttonsConfirmCancel.buttonConfirm.onClicked=onConfirm;
}