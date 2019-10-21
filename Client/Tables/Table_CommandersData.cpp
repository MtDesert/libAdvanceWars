#include"Table_CommandersData.h"
#include"GameString.h"
#include"ShapeRenderer.h"

static ShapeRenderer sr;//绘制边框用
static Rectangle2D<float> rect;//绘制矩形
static GameSprite gameSprite;//绘制头像用
static GameString gameString;//绘制文字用
static Texture texNone;

Table_CommandersData::Table_CommandersData(){
	//static
	gameString=GameString();
	gameSprite.anchorPoint.x=0;
	//必须先创建缓冲区
	uint amount=12;
	itemBuffers=new ItemBuffer[amount];

	//设定数据源
	renderItemAmount=amount;
	itemHeight=32;
	//准备要显示的表头字符串
}
Table_CommandersData::~Table_CommandersData(){
	delete []itemBuffers;
}

uint Table_CommandersData::columnAmount()const{return 2;}
uint Table_CommandersData::columnWidth(uint col)const{
	switch(col){
		case 0:return 240;
		case 1:return 560;
	}
	return 0;
}