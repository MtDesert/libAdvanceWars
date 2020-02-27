#ifndef SCENE_BATTLEFIELD_H
#define SCENE_BATTLEFIELD_H

#include"GameScene.h"
#include"Layer_BattleField.h"
#include"Menu_BattleField.h"

#include"GameButton.h"
#include"Campaign.h"

//战场的场景,负责显示战场的信息
class Scene_BattleField:public GameScene{
public:
	//构造/析构函数
	Scene_BattleField();
	~Scene_BattleField();

	//地图数据
	BattleField *battleField;//这是战场,主要存地形和单位
	Campaign *campaign;
	Campaign::CoordType cursorPoint;//光标所处的坐标

	//控件
	Layer_BattleField layerBattleField;//战场图层
	GameButton_String buttonEdit;//编辑按钮,用来打开编辑菜单

	Menu_CorpSelect menuCorpSelect;//兵种选择菜单
	Menu_TerrainSelect menuTerrainSelect;//地形选择菜单
	Menu_TroopSelect menuTroopSelect;//势力选择菜单
	Menu_CorpCommand menuCorpCommand;//兵种命令菜单,用于显示命令

	//输入事件
	virtual bool keyboardKey(Keyboard::KeyboardKey key,bool pressed);//处理光标移动
	virtual bool mouseKey(MouseKey key,bool pressed);//处理触摸效果

	//更新函数
	void updateMapRect();//更新地图矩形,影响渲染
	void updateMenu();//获取菜单情况
};
#endif