#ifndef SCENE_CAMPAIGNPREPARE_H
#define SCENE_CAMPAIGNPREPARE_H

#include"Menu_BattleField.h"
#include"Table_CampaignTroops.h"
#include"Table_CampaignWeather.h"

#include"GameScene.h"
#include"GameLayer.h"
#include"GameMenu_String.h"

//设定交战的各个部队的情况
class Layer_CampaignReady_Troop:public GameLayer{
public:
	Layer_CampaignReady_Troop();
	//列表
	Table_CampaignTroop tableCampaignTeam;//参赛队伍的列表
	//菜单
	GameMenu_String menuTeamSelect;//选择队伍的菜单
	Menu_CommanderSelect menuCommander;//选择指挥官的菜单

	virtual void reset();
};

//设置天气的界面
class Layer_CampaignReady_Weather:public GameLayer{
public:
	Layer_CampaignReady_Weather();

	Table_CampaignWeather tableCampaignWeather;

	void reset();
};

//高级战争的规则设定
class Layer_CampaignReady_Rule:public GameLayer{
public:
	Layer_CampaignReady_Rule();

	CampaignRule *campaignRule;//规则

	GameAttr_InputBoxBool attrIsFogWar;//是否开启雾战
	GameAttr_InputBoxInteger attrInitFunds;//初始资金
	GameAttr_InputBoxInteger attrBaseIncome;//据点收入
	GameAttr_InputBoxInteger attrBasesToWin;//结束战斗-据点数
	GameAttr_InputBoxInteger attrFundsToWin;//结束战斗-资金书
	GameAttr_InputBoxInteger attrTurnsToWin;//结束战斗-回合数
	GameAttr_InputBoxInteger attrUnitLevel;//单位等级上限
	GameAttr_InputBoxInteger attrCoPowerLevel;//指挥官能力等级

	void reset();
	void writeRule();
};

//比赛准备场景,在此场景中设置比赛的相关数据,包括参赛选手的设定和比赛规则的设定
class Scene_CampaignPrepare:public GameScene{
	GameString stringTitle;//标题
	GameButtonGroup_ConfirmCancel buttonsBackGO;//返回和下一步按钮

	//图层管理
	GameLayer* layersArray[3];
	int currentLayerIndex;
	void gotoSettingLayer(int delta);
public:
	Scene_CampaignPrepare();
	~Scene_CampaignPrepare();

	void reset();
};
#endif