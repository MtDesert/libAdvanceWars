#include"Scene_CommanderInfo.h"
#include"ShapeRenderer.h"
#include"Game_AdvanceWars.h"

static ShapeRenderer sr;//绘制边框用
//static Rectangle<float> rect;//绘制矩形
//static GameString gameString;//绘制文字用

Scene_CommanderInfo::Scene_CommanderInfo():source(nullptr),bodyTextures(nullptr),coIndex(0){
	/*int start0=144,start1=start0+32*3;
	txtNote.lineCharAmount=32;
	txtD2D.lineCharAmount=32;
	//名字
	stringName.setString("名字");
	stringName.position=Point2D<int>(start0,Game_AdvanceWars::currentGame()->resolution.y());
	stringName.anchorPoint=Point2D<float>(0,1);
	subObjects.push_back(&stringName);

	strName.position=stringName.position;
	strName.position.x()=start1;
	strName.anchorPoint=stringName.anchorPoint;
	subObjects.push_back(&strName);
	//口头禅
	stringQuote.setString("口癖");
	stringQuote.position=stringName.position;
	stringQuote.position.y()-=stringName.charSize.y();
	stringQuote.anchorPoint=stringName.anchorPoint;
	subObjects.push_back(&stringQuote);

	strQuote.position=stringQuote.position;
	strQuote.position.x()=start1;
	strQuote.anchorPoint=stringQuote.anchorPoint;
	subObjects.push_back(&strQuote);
	//CO说明
	stringNote.setString("说明");
	stringNote.position=stringQuote.position;
	stringNote.position.y()-=stringQuote.charSize.y();
	stringNote.anchorPoint=stringQuote.anchorPoint;
	subObjects.push_back(&stringNote);

	txtNote.position=stringNote.position;
	txtNote.position.x()=start1;
	txtNote.anchorPoint=stringNote.anchorPoint;
	subObjects.push_back(&txtNote);
	//
	stringD2D.setString("日常");
	stringD2D.position=stringNote.position;
	stringD2D.position.y()=Game_AdvanceWars::currentGame()->resolution.y()/2;
	stringD2D.anchorPoint=stringNote.anchorPoint;
	subObjects.push_back(&stringD2D);

	txtD2D.position=stringD2D.position;
	txtD2D.position.x()=start1;
	txtD2D.anchorPoint=stringD2D.anchorPoint;
	subObjects.push_back(&txtD2D);
	//全身像
	spriteBody.anchorPoint=Point2D<float>(0,0);
	subObjects.push_back(&spriteBody);*/
}
Scene_CommanderInfo::~Scene_CommanderInfo(){}

void Scene_CommanderInfo::setCoInfo(uint index){
	if(!source || !bodyTextures)return;
	uint idx=0;
	for(auto co:*source){
		if(idx==index){
			coIndex=index;
			strName.setString(co.coName);
			strQuote.setString(co.quote);
			txtNote.setString(co.note);
			txtD2D.setString(co.d2d);
			auto tex=bodyTextures->value(idx);
			if(tex)spriteBody.texture=*tex;
			break;
		}
		++idx;//继续寻找
	}
}

void Scene_CommanderInfo::keyboardKey(Keyboard::KeyboardKey key,bool pressed){
	if(!pressed && source){
		if(key==Keyboard::Key_Left){
			if(coIndex>0)setCoInfo(coIndex-1);
		}else if(key==Keyboard::Key_Right){
			if(coIndex+1<source->size())setCoInfo(coIndex+1);
		}
	}
}
