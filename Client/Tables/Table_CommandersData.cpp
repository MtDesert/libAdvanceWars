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
	gameSprite.anchorPoint.x()=0;
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

void Table_CommandersData::renderItem(const typename List<Commander>::iterator &itr,uint row,uint column,const Rectangle2D<float> &itemRect)const{
	/*bool isSelecting=(itr==itrSelecting);
	//画边框
	sr.hasFill=isSelecting;
	sr.drawRectangle(itemRect);
	//画文字
	auto center=itemRect.center();
	gameString.position.x()=center.x();
	gameString.position.y()=center.y();
	gameString.color=isSelecting?0xFF000000:0xFFFFFFFF;
	auto &item=itemBuffers[row];
	switch(column){
		case 0:gameString.stringCode=item.name;break;
		case 1:gameString.stringCode=item.quote;break;
	}
	gameString.render();
	gameString.stringCode=DataBlock();
	if(column==0){
		gameSprite.texture=item.icon;
		gameSprite.position.x()=itemRect.p0.x();
		gameSprite.position.y()=center.y();
		gameSprite.render();
	}*/
}