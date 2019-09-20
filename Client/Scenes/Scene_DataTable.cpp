#include"Scene_DataTable.h"
#include"Game_AdvanceWars.h"

Scene_DataTable::Scene_DataTable(){
	gameString_Title.anchorPoint=Point2D<float>(0.5,1);
	gameString_Title.position.y=Game::resolution.y/2;
}
Scene_DataTable::~Scene_DataTable(){}

void Scene_DataTable::setTableType(int type){
	/*subObjects.clear();
	GameSprite *table=nullptr;
	switch(type){
		case Game_AdvanceWars::File_Corps:
			table=&tableCorpData;
		break;
		case Game_AdvanceWars::File_COs:
			tableCommandersData.gotoBegin();
			table=&tableCommandersData;
		break;
		case Game_AdvanceWars::File_Troops:
			table=&tableTroopsData;
		break;
		case Game_AdvanceWars::File_Terrains:
			table=&tableTerrainsData;
		break;
		case Game_AdvanceWars::File_Weathers:
			table=&tableWeathersData;
		break;
	}
	if(table){
		table->position=Game::resolution/2;
		subObjects.push_back(table);
		subObjects.push_back(&gameString_Title);
	}*/
}

void Scene_DataTable::consumeTimeSlice(){
	/*GameSprite *table=dynamic_cast<GameSprite*>(subObjects.front());
	if(table){
		char status=GameMenuStatus::Selecting;
#define CPP_MACRO(name) if(table==&name)status=name.menuStatus;
		CPP_MACRO(tableCorpData)
		CPP_MACRO(tableCommandersData)
		CPP_MACRO(tableTroopsData)
		CPP_MACRO(tableTerrainsData)
		CPP_MACRO(tableWeathersData)
#undef CPP_MACRO
		switch(status){
			case GameMenuStatus::Cancel:
				Game::game->subObjects.remove(this);//返回上层
			break;
			case GameMenuStatus::Confirm:
				if(table==&tableCommandersData){//查看CO信息
					size_t index=0;
					for(auto item:*tableCommandersData.source){
						if(index==tableCommandersData.selectingItem){
							Game_AdvanceWars::currentGame()->gotoScene_CommanderInfo(index);
							break;
						}
						++index;
					}
				}
			break;
		}
	}*/
}
