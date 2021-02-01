#include"Scene_BattleAnimation.h"
#include"ShapeRenderer.h"
#include"Game_AdvanceWars.h"
#include"PrintF.h"

//单位默认位置
static const decltype(GameSprite::position) unitDefaultPosition[10]={
	{-64,32,0},
	{0,32,0},
	{64,32,0},
	{-96,0,0},
	{-32,0,0},
	{32,0,0},
	{96,0,0},
	{-64,-32,0},
	{0,-32,0},
	{64,-32,0},
};

#define UNIT_INFO(name)\
string##name.setString(#name,true);/*设置名称*/\
string##name.anchorPoint.x=str##name.anchorPoint.x=0;/*左对齐*/\
string##name.position.x=-size.x/2;/*往左边靠*/\
str##name.position.x = string##name.position.x +string##name.size.x +8;/*间距*/\
addSubObject(&string##name);\
string##name.addSubObject(&str##name);

Scene_BattleAnimation_UnitInfo::Scene_BattleAnimation_UnitInfo(){
	UNIT_INFO(Attack)
	UNIT_INFO(Defence)
	UNIT_INFO(Damage)
	//布局
	verticalLayout(98,0);
}
Scene_BattleAnimation_Half::Scene_BattleAnimation_Half(){
	//添加各种显示要素
	addSubObject(&background);
	for(int i=0;i<10;++i){
		addSubObject(&unitSprite[i]);
	}
	addSubObject(&headImage);
	addSubObject(&unitInfoPanel);
	//头像框
	headImage.borderColor=&ColorRGBA::White;
	headImage.bgColor=&ColorRGBA::Gray;
}
Scene_BattleAnimation::Scene_BattleAnimation(){
	//添加
	addSubObject(&leftPart);
	addSubObject(&rightPart);
	//尺寸
	auto res=Game::resolution;
	leftPart.size.setXY(res.x/2,res.y);
	rightPart.size.setP(leftPart.size);
	leftPart.borderColor=rightPart.borderColor=&ColorRGBA::White;//边框
	//位置
	rightPart.position.x=rightPart.size.x/2;
	leftPart.position.x=-rightPart.position.x;
	//基于此尺寸布局
	leftPart.refreshLayout(false);
	rightPart.refreshLayout(true);
}
Scene_BattleAnimation::~Scene_BattleAnimation(){}

void Scene_BattleAnimation_UnitInfo::reset(){
	strAttack.setString("");
	strDefence.setString("");
	strDamage.setString("");
}
void Scene_BattleAnimation_UnitInfo::refreshLayout(){
	stringAttack.position.x = stringDefence.position.x = stringDamage.position.x = -size.x/2+8;
}
void Scene_BattleAnimation_UnitInfo::setDamageData(const DamageData &data){
	auto percentFunc=[](int percent,string &str,bool showZero=false){
		if(percent!=0){
			str += percent>0 ? "+":"-";
			str += Number::toString(percent)+"\%";
		}else if(showZero){
			str += "0\%";
		}
	};
	//根据能力来显示内容
	string strAtk,strDef,strDmg;
	//攻击,反击
	percentFunc(data.coAttack,strAtk);
	percentFunc(data.baseAttack,strAtk);
	percentFunc(data.customAttack,strAtk);
	percentFunc(data.powerAttack,strAtk);
	if(strAtk.empty())percentFunc(0,strAtk,true);
	PRINT_CYAN("攻击 %d %d %d %d\n",data.coAttack,data.baseAttack,data.customAttack,data.powerAttack);
	//防御(自身,地形)
	percentFunc(data.coDefence,strDef);
	percentFunc(data.terrainDefence,strDef);
	percentFunc(data.customDefence,strDef);
	percentFunc(data.powerAttack,strDef);
	if(strDef.empty())percentFunc(0,strDef,true);
	PRINT_CYAN("防御 %d %d %d %d\n",data.coDefence,data.terrainDefence,data.customDefence,data.powerDefence);
	//损伤
	percentFunc(data.damageFix.minimun,strDmg,true);
	strDmg+="~";
	percentFunc(data.damageFix.maximun,strDmg,true);
	//设置字符串
	strAttack.setString(strAtk);
	strDefence.setString(strDef);
	strDamage.setString(strDmg);
}

void Scene_BattleAnimation_Half::reset(){
	auto tex=Texture();
	background.setTexture(tex);
	for(int i=0;i<10;++i){
		unitSprite[i].setTexture(tex);
	}
	headImage.setTexture(tex);
	GameObject::reset();
}
void Scene_BattleAnimation_Half::refreshLayout(bool isRight){
	auto halfW=size.x/2,halfH=size.y/2;
	int padding=40;
	//调整头像位置
	auto &pos=headImage.position;
	pos.setXY(halfW-padding,halfH-padding);
	if(!isRight){
		pos.x=-pos.x;
		headImage.scale.x=-1;
	}
	//调整单位方向
	for(int i=0;i<10;++i){
		unitSprite[i].position.setP(unitDefaultPosition[i]);
		if(!isRight)unitSprite[i].scale.x=-1;
	}
	//调整信息框
	unitInfoPanel.size.setXY(size.x,100);
	unitInfoPanel.position.y=-halfH;
	unitInfoPanel.anchorPoint.y=0;
	unitInfoPanel.bgColor=&ColorRGBA::Gray;
	unitInfoPanel.borderColor=&ColorRGBA::White;
	unitInfoPanel.refreshLayout();
}
void Scene_BattleAnimation_Half::setDamageData(const DamageData &damageData){
	GAME_AW
	//设置背景
	background.setTexture_FilePNG("images/TerrainsBG/Plain.png",Game_AdvanceWars::whenError);
	//设置单位
	auto ud=damageData.unitData;
	if(!ud)return;
	if(ud->unit && ud->campaignTroop){
		unitSprite[0].setTexture(game->corpsImagesArray.getTexture(ud->unit->corpType,ud->campaignTroop->troopID));
		for(int i=1;i<10;++i){
			unitSprite[i].setTexture(unitSprite[0].texture);
		}
	}
	//设置头像
	if(ud->campaignCO){
		headImage.setTexture(game->commandersHeadTextures.getTexture(ud->campaignCO->coID));
	}
	//显示累加数据
	unitInfoPanel.setDamageData(damageData);
}

bool Scene_BattleAnimation::keyboardKey(Keyboard::KeyboardKey key, bool pressed){
	switch(key){
		case Keyboard::Key_Esc:{
			GAME_AW
			game->gotoScene_BattleField();
		}break;
		default:;
	}
	return true;
}
void Scene_BattleAnimation::playAnimation(const Campaign &campaign){
	leftPart.setDamageData(campaign.damageCaculator->attackerDamageData);
	rightPart.setDamageData(campaign.damageCaculator->defenderDamageData);
}
