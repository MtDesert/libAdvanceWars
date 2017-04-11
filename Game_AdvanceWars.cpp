#include"Game_AdvanceWars.h"
#include"GameScene_Main.h"

Game_AdvanceWars::Game_AdvanceWars(){
	auto scene=new GameScene_Main();
	subObjects.push_back(scene);
}
Game_AdvanceWars::~Game_AdvanceWars(){}

Game* Game::newGame(){return new Game_AdvanceWars();}