#ifndef DIALOG_NEWMAP_H
#define DIALOG_NEWMAP_H

#include"GameDialog.h"
#include"GameInputBox.h"

#include"BattleField.h"

//新建地图的对话框
class Dialog_NewMap:public GameDialog{
public:
	Dialog_NewMap();
	//控件
	GameString stringMapInfo;//标题
	GameAttr_InputBoxString attrMapName;//地图名
	GameAttr_InputBoxString attrAuthor;//作者
	GameAttr_InputBoxInteger attrWidth;//宽度
	GameAttr_InputBoxInteger attrHeight;//高度
	GameButtonGroup_ConfirmCancel buttonsConfirmCancel;//确认取消按钮

	void setBattleField(const BattleField &battleField);//给控件设置上battleField的信息
	void resetBattleField(BattleField &battleField)const;//根据玩家输入的参数来设置战场信息
	void setConfirmCallback(GameButton::ClickCallback onConfirm);//设置确定时候的回调函数
};
#endif