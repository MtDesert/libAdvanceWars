#ifndef SCENE_BATTLEFIELD_H
#define SCENE_BATTLEFIELD_H

#include"BattleField.h"

#include"Scene_DataTable.h"
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
	//纹理数据
	TextureCache *terrainsTextures;//地形纹理
	TextureCache *corpsTextures;//兵种纹理

	//输入事件
	virtual bool keyboardKey(Keyboard::KeyboardKey key,bool pressed);//处理光标移动
	virtual bool mouseKey(MouseKey key,bool pressed);//处理触摸效果
	virtual bool mouseMove(int x,int y);//处理光标跟随鼠标效果
	//渲染函数
	virtual void renderX()const;//画战场
	virtual Point2D<float> sizeF()const;//根据地图规格来判断尺寸
protected:
	//光标定位
	void cursorSelect();//根据选择后的结果来刷新显存
};
#endif