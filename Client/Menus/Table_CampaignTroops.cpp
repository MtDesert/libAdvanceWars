#include"Table_CampaignTroops.h"
#include"Number.h"

#define ITEM_WIDTH 480
#define ICON_SIZE 32

TableItem_CampaignTroop::TableItem_CampaignTroop(){
	size.setXY(ITEM_WIDTH,ICON_SIZE);
	//调整尺寸
	spriteTeamIcon.size.setXY(ICON_SIZE,ICON_SIZE);
	buttonIsAI.setString("    ");//约4个字符的宽度
	buttonIsAI.resizeAsString();
	buttonTeam.setString("TeamA ");
	buttonTeam.resizeAsString();
	inputBoxCOamount.setString("CO*0");
	inputBoxCOamount.resizeAsString();
	inputBoxCOamount.maxInteger=MENU_CAMPAIGNTROOPS_MAX_CO_COUNT;
	//添加
	addSubObject(&spriteTeamIcon);
	addSubObject(&buttonIsAI);
	addSubObject(&buttonTeam);
	addSubObject(&inputBoxCOamount);
	for(int i=0;i<MENU_CAMPAIGNTROOPS_MAX_CO_COUNT;++i){
		buttonCOs[i].size.setXY(ICON_SIZE,ICON_SIZE);
		addSubObject(&buttonCOs[i]);
	}
	//布局
	horizontalLayout(-size.x/2 + ICON_SIZE,0);
}
Table_CampaignTroop::Table_CampaignTroop():allTroops(nullptr),troopsTextures(nullptr),commandersTextures(nullptr),
onItemTeamButtonClicked(nullptr),onItemCObuttonClicked(nullptr){
	pSpriteSelector=new GameSprite();
	pSpriteSelector->position.x=-ITEM_WIDTH/2 + ICON_SIZE/2;
}
string Table_CampaignTroop::translateOf_Team;

void TableItem_CampaignTroop::updateData(SizeType pos){
	auto menu=dynamic_cast<Table_CampaignTroop*>(parentObject);
	if(!menu)return;
	auto pTroop=menu->allTroops->data(pos);
	if(!pTroop)return;
	//设置各项数据
	spriteTeamIcon.setTexture(menu->troopsTextures->getTexture(pTroop->troopID));
	buttonIsAI.setString(pTroop->isAI ? "COM" : "PLR",true);
	buttonTeam.setString(Table_CampaignTroop::strTeamName(pTroop->teamID));
	inputBoxCOamount.setString("CO*"+Number::toString(pTroop->allCOs.size()));
	//调整CO控件数量
	for(SizeType i=0;i<MENU_CAMPAIGNTROOPS_MAX_CO_COUNT;++i){
		auto &btn=buttonCOs[i];
		removeSubObject(&btn);
		auto co=pTroop->allCOs.data(i);
		if(co){
			addSubObject(&btn);
			//读取CO数据
			btn.setTexture(menu->commandersTextures->getTexture(co->coID));
			btn.onClicked=[menu,co](){
				if(menu->onItemCObuttonClicked)menu->onItemCObuttonClicked(*co);
			};
		}
	}
	//事件
	buttonIsAI.onClicked=[menu,pTroop](){
		pTroop->isAI = !pTroop->isAI;//立刻改变状态
		menu->updateRenderParameters(true);
	};
	buttonTeam.onClicked=[menu,pTroop](){//弹出菜单
		if(menu->onItemTeamButtonClicked)menu->onItemTeamButtonClicked(*pTroop);
	};
	inputBoxCOamount.whenInputConfirm=[&,menu,pTroop](){
		pTroop->allCOs.setSize(inputBoxCOamount.mInteger,true);
		menu->updateRenderParameters(true);
	};
}

static SizeType colWidths[]={ICON_SIZE*2,ICON_SIZE*2,ICON_SIZE*3,ICON_SIZE*2,192};
SizeType Table_CampaignTroop::columnAmount()const{return 5;}
SizeType Table_CampaignTroop::columnWidth(SizeType column)const{return column < columnAmount() ? colWidths[column] : 0;}
SizeType Table_CampaignTroop::itemAmount()const{return allTroops ? allTroops->size() : 0;}
void Table_CampaignTroop::updateSize(){
	auto &item=itemArray[0];
	size.setXY(item.size.x,item.size.y*renderItemAmount);
}

string Table_CampaignTroop::strTeamName(int teamID){
	char ch[6];
	if(translateOf_Team=="Team"){
		sprintf(ch,"Team%c",'A'+teamID);
	}else{
		sprintf(ch,"%c%s",'A'+teamID,translateOf_Team.data());
	}
	return ch;
}