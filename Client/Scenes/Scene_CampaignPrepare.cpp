#include"Scene_CampaignPrepare.h"
#include"Game_AdvanceWars.h"

//队伍设定
Layer_CampaignReady_Troop::Layer_CampaignReady_Troop(){
	//菜单
	tableCampaignTeam.setRenderItemAmount(8);
	menuTeamSelect.setRenderItemAmount(8);
	//添加
	addSubObject(&tableCampaignTeam);
}
void Layer_CampaignReady_Troop::reset(){
	//展示出可参赛的队伍
	GAME_AW
	game->campaign.makeAllTeams();
	game->campaign.campaignWeathers.setWeatherAmount(game->mWeathersList.size());
	//设置翻译用的内容
	if(Table_CampaignTroop::translateOf_Team.empty()){
		Table_CampaignTroop::translateOf_Team=game->translate("Team");
	}
	tableCampaignTeam.pSpriteSelector->setTexture(game->texMenuArrow);
	tableCampaignTeam.allTroops = &game->campaign.allTroops;
	tableCampaignTeam.troopsTextures = &game->troopsTextures;
	tableCampaignTeam.commandersTextures = &game->commandersHeadTextures;
	tableCampaignTeam.updateRenderParameters(true);
	menuCommander.pSpriteSelector->setTexture(game->texMenuArrow);
	//生成选择队伍的菜单
	menuTeamSelect.gameStringList.clear();
	menuTeamSelect.setRenderItemAmount(8);
	int troopCount=game->campaign.allTroops.size();
	for(int i=0;i<troopCount;++i){
		menuTeamSelect.addString(Table_CampaignTroop::strTeamName(i));
	}
	menuTeamSelect.updateRenderParameters(true);
	//让每只部队都能选择所属分组
	tableCampaignTeam.onItemTeamButtonClicked=[&](CampaignTroop &troop){
		//显示选分组菜单
		menuTeamSelect.selectingItemIndex=troop.teamID;
		menuTeamSelect.updateRenderParameters(true);
		addSubObject(&menuTeamSelect);
		//选择后,写回数据
		menuTeamSelect.onConfirm=[&](GameMenu*){
			menuTeamSelect.removeFromParentObject();//菜单消失
			troop.teamID=menuTeamSelect.selectingItemIndex;//数据写回
			tableCampaignTeam.updateRenderParameters(true);//刷新显示
		};
	};
	tableCampaignTeam.onItemCObuttonClicked=[&](CampaignCO &co){
		//显示菜单
		menuCommander.selectingItemIndex=co.coID;
		menuCommander.updateRenderParameters(true);
		addSubObject(&menuCommander);
		//选择后,写回数据
		menuCommander.onConfirm=[&](GameMenu*){
			menuCommander.removeFromParentObject();
			co.coID=menuCommander.selectingItemIndex;
			tableCampaignTeam.updateRenderParameters(true);
		};
	};
}

//天气设定
Layer_CampaignReady_Weather::Layer_CampaignReady_Weather(){
	tableCampaignWeather.setRenderItemAmount(7);
	addSubObject(&tableCampaignWeather);
}
void Layer_CampaignReady_Weather::reset(){
	GAME_AW
	tableCampaignWeather.weathersList = &game->mWeathersList;
	tableCampaignWeather.campaignWeathers = &game->campaign.campaignWeathers;
	tableCampaignWeather.updateRenderParameters(true);
}

//规则设定
#define ALL_RULES(MACRO) \
MACRO(IsFogWar)\
MACRO(InitFunds)\
MACRO(BaseIncome)\
MACRO(BasesToWin)\
MACRO(FundsToWin)\
MACRO(TurnsToWin)\
MACRO(UnitLevel)\
MACRO(CoPowerLevel)\

Layer_CampaignReady_Rule::Layer_CampaignReady_Rule():campaignRule(nullptr){
#define SET_NAME_AND_ADD(name) \
attr##name.setLabelName_ValueWidth_MaxWidth(#name,true,160,480);\
attr##name.stringLabel.setString(#name,true);\
addSubObject(&attr##name);
	ALL_RULES(SET_NAME_AND_ADD)
	//默认字符串
	attrIsFogWar.inputBox.trueStr="ON";
	attrIsFogWar.inputBox.falseStr="OFF";
	//调整布局
	verticalLayout(attrIsFogWar.size.y * subObjects.size()/2,0);
}

void Layer_CampaignReady_Rule::reset(){
	GAME_AW
	campaignRule = &game->campaign.campaignRule;
#define READ_RULE(name) attr##name.inputBox.setValue(campaignRule->m##name);
	ALL_RULES(READ_RULE)
#undef READ_RULE
}
void Layer_CampaignReady_Rule::writeRule(){
#define WRITE_RULE(name) campaignRule->m##name=attr##name.getValue();
	ALL_RULES(WRITE_RULE)
#undef WRITE_RULE
}

//赛前准备场景
#define LAYER_COUNT 3

Scene_CampaignPrepare::Scene_CampaignPrepare(){
	layersArray[0]=new Layer_CampaignReady_Troop;
	layersArray[1]=new Layer_CampaignReady_Weather;
	layersArray[2]=new Layer_CampaignReady_Rule;
	currentLayerIndex=0;
	//标题
	stringTitle.setString("TroopsFiles",true);
	stringTitle.anchorPoint.y = 1;
	stringTitle.position.y = Game::resolution.y/2;
	//确定返回按钮
	buttonsBackGO.position.y=-Game::resolution.y/2 + buttonsBackGO.size.y;
	addSubObject(&buttonsBackGO);
	//事件
	buttonsBackGO.setConfirmCancelFunction([&](){
		gotoSettingLayer(1);
	},[&](){
		gotoSettingLayer(-1);
	});
}
Scene_CampaignPrepare::~Scene_CampaignPrepare(){
	for(int i=0;i<LAYER_COUNT;++i)deleteSubObject(layersArray[i]);
}

void Scene_CampaignPrepare::gotoSettingLayer(int delta){
	removeSubObject(layersArray[currentLayerIndex]);
	currentLayerIndex += delta;
	//跳转
	GAME_AW
	if(currentLayerIndex<0){
		currentLayerIndex=0;
		game->gotoScene_FileList();
	}else if(currentLayerIndex >= LAYER_COUNT){
		currentLayerIndex=LAYER_COUNT-1;
		//写入各种设定数据,然后跳转战场
		auto layer=dynamic_cast<Layer_CampaignReady_Rule*>(layersArray[2]);
		layer->writeRule();//把设定的规则写入
		auto scene=game->gotoScene_BattleField(true);
		scene->gotoBattleMode();
	}else{
		addSubObject(layersArray[currentLayerIndex]);
	}
}

void Scene_CampaignPrepare::reset(){
	for(int i=0;i<LAYER_COUNT;++i)layersArray[i]->reset();
	currentLayerIndex=0;
	addSubObject(layersArray[currentLayerIndex]);
}