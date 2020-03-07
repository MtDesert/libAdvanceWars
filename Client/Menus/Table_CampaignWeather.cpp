#include"Table_CampaignWeather.h"
#include"Number.h"

#define ICON_SIZE 32

TableItem_CampaignWeather::TableItem_CampaignWeather(){
	size.setXY(480,ICON_SIZE);
	stringName.setString("????");
	inputBoxOccurrenceFactor.setString("????");
	inputBoxOccurrenceFactor.maxInteger=9999;
	inputBoxOccurrenceFactor.resizeAsString();
	stringOccurrenceRate.size.setXY(size.x - stringName.size.x - inputBoxOccurrenceFactor.size.x,ICON_SIZE);
	//添加
	addSubObject(&stringName);
	addSubObject(&inputBoxOccurrenceFactor);
	addSubObject(&stringOccurrenceRate);
	horizontalLayout(-size.x/2,0);
}
Table_CampaignWeather::Table_CampaignWeather(){}

void TableItem_CampaignWeather::updateData(SizeType pos){
	auto menu=dynamic_cast<Table_CampaignWeather*>(parentObject);
	if(!menu || !menu->weathersList || !menu->campaignWeathers)return;
	//取数值
	auto weather=menu->weathersList->data(pos);
	auto dt=menu->campaignWeathers->weatherData.data(pos);
	if(!dt || !weather)return;
	//显示数值
	inputBoxOccurrenceFactor.whenInputConfirm=nullptr;//注:一定要有此语句,否则可能会无限触发
	stringName.setString(weather->translate);
	inputBoxOccurrenceFactor.setValue(dt->factor);
	stringOccurrenceRate.setString(Number::double2PercentString(dt->rate));
	//事件
	inputBoxOccurrenceFactor.whenInputConfirm=[&,menu,pos](){
		menu->campaignWeathers->setWeatherFactor(pos,inputBoxOccurrenceFactor.mInteger);
		menu->updateRenderParameters(true);
	};
}

static SizeType colWidths[]={ICON_SIZE*2,ICON_SIZE*2,ICON_SIZE*11};
SizeType Table_CampaignWeather::columnAmount()const{return 3;}
SizeType Table_CampaignWeather::columnWidth(SizeType column)const{return colWidths[column];}
SizeType Table_CampaignWeather::itemAmount()const{return campaignWeathers ? campaignWeathers->weatherData.size() : 0;}