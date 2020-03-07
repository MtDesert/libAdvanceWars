#ifndef TABLE_CAMPAIGNWEATHER_H
#define TABLE_CAMPAIGNWEATHER_H

#include"GameTable.h"
#include"GameInputBox.h"
#include"Campaign.h"
#include"Weather.h"

class TableItem_CampaignWeather:public GameMenuItem{
public:
	TableItem_CampaignWeather();

	GameString stringName;//天气名
	GameInputBox_Integer inputBoxOccurrenceFactor;//出现基数
	GameString stringOccurrenceRate;//出现率

	virtual void updateData(SizeType pos);
};

//比赛天气列表
class Table_CampaignWeather:public GameTableTemplate<TableItem_CampaignWeather>{
public:
	Table_CampaignWeather();

	//override
	virtual SizeType columnAmount()const;
	virtual SizeType columnWidth(SizeType column)const;
	virtual SizeType itemAmount()const;

	//数据源
	WeathersList *weathersList;
	CampaignWeathers *campaignWeathers;
};
#endif//MENU_CAMPAIGNWEATHER_H