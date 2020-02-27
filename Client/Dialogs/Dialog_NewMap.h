#ifndef DIALOG_NEWMAP_H
#define DIALOG_NEWMAP_H

#include"GameDialog.h"
#include"GameInputBox.h"
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
};
#endif