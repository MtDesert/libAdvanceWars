#ifndef SPRITE_CAMPAIGNINFO_H
#define SPRITE_CAMPAIGNINFO_H

#include"GameString.h"
#include"Campaign.h"

/*显示当前天数和行动部队*/
class Sprite_CurrentDay:public GameSprite{
	GameString stringName;//部队名字
	GameString stringDay;//当前天数
public:
	Sprite_CurrentDay();
	//设置信息
	void setCampaign(const Campaign &campaign);
};

/*显示当前行动的部队和CO等简略信息*/
class Sprite_TroopFundsCO:public GameSprite{
public:
	Sprite_TroopFundsCO();

	GameSprite headImage;//当前指挥官的头像
	GameString stringFunds;//当前部队的资金
	GameString stringEnergy;//当前部队的能量槽

	void setCampaignTroop(const CampaignTroop &troop);
};

//地形信息
class Sprite_TerrainInfo:public GameSprite{
public:
	Sprite_TerrainInfo();

	void setUnitData(const UnitData &unitData);//设置显示数据

	//控件相关
	ColorRGBA bgColor;//背景色
	GameSprite terrainIcon;//地形图标
	GameString terrainName;//地形名
	GameSprite starIcon;//星星图标
	GameString starNum;//星星数量
};

//单位信息,简略显示单位情况
class Sprite_UnitInfo:public GameSprite{
public:
	Sprite_UnitInfo();

	ColorRGBA bgColor;//背景色
	//各个显示控件
	GameSprite unitIcon;//单位图标
	GameString unitName;//单位名
	//HP,燃料,弹药
	GameSprite iconHP,iconGas,iconAmmo;
	GameString stringHP,stringGas,stringAmmo;

	void setUnitData(const UnitData &unitData);//显示单位数据
};

#endif//SPRITE_CAMPAIGNINFO_H