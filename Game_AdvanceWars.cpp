#include"Game_AdvanceWars.h"
#include"GameScene_Main.h"

Game_AdvanceWars::Game_AdvanceWars(){
	auto scene=new GameScene_Main();
	allScenes.push_back(scene);
	currentScene=scene;
}
Game_AdvanceWars::~Game_AdvanceWars(){}

Game* Game::newGame(){return new Game_AdvanceWars();}