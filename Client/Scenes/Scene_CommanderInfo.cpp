#include"Scene_CommanderInfo.h"

Scene_CommanderInfo::Scene_CommanderInfo():source(nullptr),coIndex(0){
	/*int start0=144,start1=start0+32*3;
	txtNote.lineCharAmount=32;
	txtD2D.lineCharAmount=32;
	//名字
	stringName.setString("名字");
	stringName.position=Point3D<int>(start0,Game_AdvanceWars::currentGame()->resolution.y,0);
	stringName.anchorPoint=Point2D<float>(0,1);
	addSubObject(&stringName);

	strName.position=stringName.position;
	strName.position.x=start1;
	strName.anchorPoint=stringName.anchorPoint;
	addSubObject(&strName);
	//口头禅
	stringQuote.setString("口癖");
	stringQuote.position=stringName.position;
	stringQuote.position.y-=stringName.charSize.y;
	stringQuote.anchorPoint=stringName.anchorPoint;
	addSubObject(&stringQuote);

	strQuote.position=stringQuote.position;
	strQuote.position.x=start1;
	strQuote.anchorPoint=stringQuote.anchorPoint;
	addSubObject(&strQuote);
	//CO说明
	stringNote.setString("说明");
	stringNote.position=stringQuote.position;
	stringNote.position.y-=stringQuote.charSize.y;
	stringNote.anchorPoint=stringQuote.anchorPoint;
	addSubObject(&stringNote);

	txtNote.position=stringNote.position;
	txtNote.position.x=start1;
	txtNote.anchorPoint=stringNote.anchorPoint;
	addSubObject(&txtNote);
	//
	stringD2D.setString("日常");
	stringD2D.position=stringNote.position;
	stringD2D.position.y=Game_AdvanceWars::currentGame()->resolution.y/2;
	stringD2D.anchorPoint=stringNote.anchorPoint;
	addSubObject(&stringD2D);

	txtD2D.position=stringD2D.position;
	txtD2D.position.x=start1;
	txtD2D.anchorPoint=stringD2D.anchorPoint;
	addSubObject(&txtD2D);
	//全身像
	spriteBody.anchorPoint=Point2D<float>(0,0);
	addSubObject(&spriteBody);*/
}
Scene_CommanderInfo::~Scene_CommanderInfo(){}

void Scene_CommanderInfo::setCoInfo(uint index){
	if(!source)return;
	uint idx=0;
	for(auto co:*source){
		if(idx==index){
			coIndex=index;
			strName.setString(co.name);
			strQuote.setString(co.quote);
			txtNote.setString(co.note);
			txtD2D.setString(co.d2d);
			break;
		}
		++idx;//继续寻找
	}
}