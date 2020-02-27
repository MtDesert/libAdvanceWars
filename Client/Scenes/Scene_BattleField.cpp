#include"Scene_BattleField.h"
#include"Sprite_Unit.h"
#include"Game_AdvanceWars.h"

#include"extern.h"

//渲染变量
static Game_AdvanceWars *game=nullptr;

Scene_BattleField::Scene_BattleField():battleField(nullptr){
	game=Game_AdvanceWars::currentGame();
	//战场图层
	layerBattleField.battleField=&game->battleField;
	addSubObject(&layerBattleField);
	//编辑菜单按钮
	buttonEdit.setString("Edit");
	addSubObject(&buttonEdit);

	battleField=&game->battleField;//读取数据用
	campaign=&game->campaign;

	//菜单样式
#define MENU_INIT(Name) \
menu##Name.bgColor = &ColorRGBA::Green;\
menu##Name.borderColor = &ColorRGBA::White;\
menu##Name.pSpriteSelector->texture = game->texMenuArrow;

	MENU_INIT(CorpSelect)
	MENU_INIT(TerrainSelect)
	MENU_INIT(TroopSelect)
	MENU_INIT(CorpCommand)
	//菜单事件
}
Scene_BattleField::~Scene_BattleField(){}

bool Scene_BattleField::keyboardKey(Keyboard::KeyboardKey key,bool pressed){
	bool ret=false;
	if(!pressed){
		ret=true;
		int x=0,y=0;
		switch(key){
			case Keyboard::Key_Up:y=1;break;
			case Keyboard::Key_Down:y=-1;break;
			case Keyboard::Key_Left:x=-1;break;
			case Keyboard::Key_Right:x=1;break;
			case Keyboard::Key_Enter:
				campaign->cursorSelect();
				updateMenu();
			break;
			case Keyboard::Key_Esc:
				campaign->cursorCancel();
			break;
			//debug
			case Keyboard::Key_F1:
				addSubObject(&menuCorpSelect);
				menuCorpSelect.updateRenderParameters();
			break;
			case Keyboard::Key_F2:
				addSubObject(&menuTerrainSelect);
				menuTerrainSelect.updateRenderParameters();
			break;
			case Keyboard::Key_F3:
				addSubObject(&menuTroopSelect);
				menuTroopSelect.updateRenderParameters();
			break;
			default:ret=false;
		}
		//移动方向
		if(x || y){
			campaign->setCursor(campaign->cursor.x+x,campaign->cursor.y+y);
		}
	}
	return ret;
}

//拖动效果变量
static decltype(Game::mousePos) touchBeginPoint,touchMovePoint;//触摸起点,移动中的点
static bool mouseKeyDown=false;//是否按下鼠标
static bool isTouchMove=false;//是否拖动行为

bool Scene_BattleField::mouseKey(MouseKey key,bool pressed){
	if(key==Mouse_LeftButton){
		if(pressed){//点击时候记录位置
			touchBeginPoint = Game::mousePos;
			touchMovePoint = Game::mousePos;
		}else if(isTouchMove);//如果是拖动,放手后一般不处理
		else{//处理点击事件
			campaign->cursorSelect();
			updateMenu();
		}
		//改变状态
		mouseKeyDown=pressed;
		isTouchMove=false;
	}else if(key==Mouse_RightButton && !pressed){
		campaign->cursorCancel();
	}
	return false;
}

void Scene_BattleField::updateMapRect(){
	layerBattleField.updateMapRect();
}
void Scene_BattleField::updateMenu(){
	if(!menuCorpCommand.parentObject && campaign->corpMenu.size()){
		//debug
		for(int i=0;i<8;++i){
			campaign->corpMenu.push_back(i);
		}
		//动态生成兵种命令菜单
		addSubObject(&menuCorpCommand);
		menuCorpCommand.updateRenderParameters();
	}
	if(menuCorpCommand.parentObject && campaign->corpMenu.size()==0){
		removeSubObject(&menuCorpCommand);
	}
}