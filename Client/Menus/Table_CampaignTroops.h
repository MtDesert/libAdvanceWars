#ifndef TABLE_CAMPAIGNTROOP_H
#define TABLE_CAMPAIGNTROOP_H

#include"GameTable.h"
#include"GameInputBox.h"
#include"Campaign.h"

#define MENU_CAMPAIGNTROOPS_MAX_CO_COUNT 6

//参赛队伍的Item
class TableItem_CampaignTroop:public GameMenuItem{
public:
	TableItem_CampaignTroop();

	GameSprite spriteTeamIcon;//参赛队伍的图标
	GameButton_String buttonIsAI;//参赛队伍是人脑还是电脑
	GameButton_String buttonTeam;//参赛队伍的分组状态
	GameInputBox_Integer inputBoxCOamount;//CO数量设置
	GameButton buttonCOs[6];//每个CO的具体样貌,点击可以修改

	virtual void updateData(SizeType pos);
};

//参赛队伍的菜单,将参赛队伍以菜单的形式展现出来
class Table_CampaignTroop:public GameTableTemplate<TableItem_CampaignTroop>{
public:
	Table_CampaignTroop();

	Array<CampaignTroop> *allTroops;//指向数据源
	TextureCache *troopsTextures;//势力纹理
	TextureCache *commandersTextures;//指挥官头像

	//override
	virtual SizeType columnAmount()const;
	virtual SizeType columnWidth(SizeType column)const;
	virtual SizeType itemAmount()const;

	static string strTeamName(int teamID);//根据teamID生成对应的字符串
	static string translateOf_Team;//"Team"的译文
	function<void(CampaignTroop&)> onItemTeamButtonClicked;//点击参赛队伍的分组按钮,则调用此函数
	function<void(CampaignCO&)> onItemCObuttonClicked;//点击CO头像,更换CO
protected:
	virtual void updateSize();
};
#endif//MENU_CAMPAIGNTEAM_H