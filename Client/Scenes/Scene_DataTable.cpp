#include"Scene_DataTable.h"
#include"Game_AdvanceWars.h"

Scene_DataTable::Scene_DataTable(){
	gameString_Title.anchorPoint=Point2D<float>(0.5,1);
	gameString_Title.position.y=Game::resolution.y/2;
}
Scene_DataTable::~Scene_DataTable(){}