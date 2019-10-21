#ifndef SCENE_BATTLEFIELD_H
#define SCENE_BATTLEFIELD_H

#include"BattleField.h"
#include"Campaign.h"

#include"Scene_DataTable.h"
//#include"GameMenu_String.h"
#include"GameScene.h"
#include"Texture.h"

//战场的场景,负责显示战场的信息
class Scene_BattleField:public GameScene{
public:
	//构造/析构函数
	Scene_BattleField();
	~Scene_BattleField();

	//地图数据
	BattleField *battleField;//这是战场,主要存地形和单位
	Campaign *campaign;//这是比赛,主要存参赛对手信息以及行动规则
	//纹理数据
	TextureCache *terrainsTextures;//地形纹理
	TextureCache *corpsTextures;//兵种纹理

	//输入事件
	virtual void keyboardKey(Keyboard::KeyboardKey key,bool pressed);//处理光标移动
	virtual void mouseKey(MouseKey key,bool pressed);//处理触摸效果
	virtual void mouseMove(int x,int y);//处理光标跟随鼠标效果
	//渲染函数
	virtual void renderX()const;//画战场
	virtual Point2D<float> sizeF()const;//根据地图规格来判断尺寸
protected:
	virtual void consumeTimeSlice();//处理菜单操作
	//渲染
	void renderTerrainInfo()const;//渲染地形信息
	void renderUnitInfo()const;//渲染兵种信息
	void renderTeamInfo()const;//渲染队伍信息
	//光标定位
	Point2D<int> cursorPos()const;//光标的物理坐标(左下角),与镜头的position无关
	void setCursorPos(int x,int y);//根据物理坐标修改光标的位置,并且获取地形和单位信息
	void cursorSelect();//根据选择后的结果来刷新显存
	void getCursorInfo()const;//获取光标处的信息
};

#endif
