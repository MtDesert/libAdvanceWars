#ifndef SCENE_DATATABLE_H
#define SCENE_DATATABLE_H

#include"GameString.h"
/*#include"Table_CorpsData.h"
#include"Table_CommandersData.h"
#include"Table_TroopsData.h"
#include"Table_TerrainsData.h"
#include"Table_WeathersData.h"*/

#include"GameScene.h"

/*高战的数据表,用来显示表格内容*/
class Scene_DataTable:public GameScene{
public:
	Scene_DataTable();
	~Scene_DataTable();

	//标题
	GameString gameString_Title;
	//视图,仅仅用于显示
	/*Table_CorpsData tableCorpData;
	Table_CommandersData tableCommandersData;
	Table_TroopsData tableTroopsData;
	Table_TerrainsData tableTerrainsData;
	Table_WeathersData tableWeathersData;*/
};
#endif