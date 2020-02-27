#include "Dialog_NewMap.h"

static const SizeType spacing=16;//间隔
Dialog_NewMap::Dialog_NewMap(){
	const SizeType itemWidth=400;
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

	//debug
	buttonsConfirmCancel.setConfirmCancelFunction([&](){
		printf("Confirm %s %s %d %d\n",attrMapName.getValue().data(),attrAuthor.getValue().data(),attrWidth.getValue(),attrHeight.getValue());
	},[](){
		printf("Cancel\n");
	});
}