#include"Sprite_CampaignInfo.h"
#include"Game_AdvanceWars.h"
#include"Number.h"

//天数,行动方
Sprite_CurrentDay::Sprite_CurrentDay(){
	bgColor=&ColorRGBA::Black;
	borderColor=&ColorRGBA::White;
	size.setXY(256,80);
	//位置
	stringName.position.setXY(0,16);
	stringDay.position.setXY(0,-16);
	//添加
	addSubObject(&stringName);
	addSubObject(&stringDay);
}

void Sprite_CurrentDay::setCampaign(const Campaign &campaign){
	stringDay.setString("Day "+Number::toString(campaign.currentDay));
	auto pTroop=campaign.currentTroop();
	if(pTroop){
		GAME_AW
		auto troop=game->mTroopsList.data(pTroop->troopID);
		if(troop){
			stringName.setString(troop->translate);
			stringName.insertTexture(game->troopsTextures.getTexture(pTroop->troopID),0);
		}
	}
}

//部队资金CO
Sprite_TroopFundsCO::Sprite_TroopFundsCO(){
	size.setXY(256,64);
	borderColor=&ColorRGBA::White;
	bgColor=&ColorRGBA::Black;
	//位置
	headImage.size.setXY(64,64);
	headImage.position.x = -size.x/2 + 32;
	stringFunds.anchorPoint.x = stringEnergy.anchorPoint.x = 0;
	stringFunds.position.setXY(-size.x/2 + headImage.size.x,16);
	stringEnergy.position.setXY(-size.x/2 + headImage.size.x,-16);
	//添加
	addSubObject(&headImage);
	addSubObject(&stringFunds);
	addSubObject(&stringEnergy);
}

void Sprite_TroopFundsCO::setCampaignTroop(const CampaignTroop &troop){
	GAME_AW
	//显示CO头像
	auto co=troop.allCOs.firstData();
	if(co){
		headImage.setTexture(game->commandersHeadTextures.getTexture(co->coID));
		stringEnergy.setString(Number::toString(co->energy));
	}
	stringFunds.setString(Number::toString(troop.funds)+"G");
}

//地形信息
Sprite_TerrainInfo::Sprite_TerrainInfo(){
	size.setXY(160,128);
	GameSprite::bgColor=&bgColor;
	bgColor=ColorRGBA(0,0,0,128);
	borderColor=&ColorRGBA::White;
	//各个控件位置
	terrainName.position.y=48;
	starIcon.position.setXY(-16,-48);
	starNum.position.x=32;

	addSubObject(&terrainIcon);
	addSubObject(&terrainName);
	addSubObject(&starIcon);
	starIcon.addSubObject(&starNum);
}

void Sprite_TerrainInfo::setUnitData(const UnitData &unitData){
	auto code=unitData.terrainCode;
	auto trn=unitData.terrain;
	if(code && trn){
		GAME_AW
		terrainIcon.setTexture(game->terrainsTexturesArray.getTexture(trn->terrainType,trn->status));
		terrainName.setString(code->translate);
		starNum.setString(Number::toString(code->defendLV));
	}
}

//单位信息
Sprite_UnitInfo::Sprite_UnitInfo(){
	size.setXY(160,160);
	GameSprite::bgColor=&bgColor;
	bgColor=ColorRGBA(0,0,0,128);
	borderColor=&ColorRGBA::White;
	//调整大小
	unitName.setRawString("");
	unitIcon.size.y = unitName.size.y;
	//各个控件位置
	stringHP.anchorPoint.x=stringGas.anchorPoint.x=stringAmmo.anchorPoint.x=1;
	stringHP.position.x=stringGas.position.x=stringAmmo.position.x=64;
	iconHP.position.x=iconGas.position.x=iconAmmo.position.x=-24;
	iconGas.position.y=-32;
	iconAmmo.position.y=-64;
	//添加
	addSubObject(&unitName);
	addSubObject(&unitIcon);
	addSubObject(&iconHP);
	addSubObject(&iconGas);
	addSubObject(&iconAmmo);
	iconHP.addSubObject(&stringHP);
	iconGas.addSubObject(&stringGas);
	iconAmmo.addSubObject(&stringAmmo);
}

void Sprite_UnitInfo::setUnitData(const UnitData &unitData){
	auto unit=unitData.unit;
	auto corp=unitData.corp;
	if(unit && corp){
		setColor(ColorRGBA::White);
		GAME_AW
		unitIcon.setTexture(game->corpsIconsArray.getTexture(unit->corpType,unit->color));
		unitName.setString(corp->translate);
		stringHP.setString(Number::toString(unit->healthPower));
		stringGas.setString(Number::toString(unit->fuel));
		stringAmmo.setString(Number::toString(unit->ammunition));
	}else{
		setColor(ColorRGBA::Transparent);
	}
}