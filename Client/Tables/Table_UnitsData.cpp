#include"Table_UnitsData.h"
#include"GameString.h"
#include"ShapeRenderer.h"

#include"Charset.h"

static ShapeRenderer sr;//绘制边框用
static Rectangle2D<float> rect;//绘制矩形
static GameString gameString;//绘制文字用

/*static const char* headerName[]={
	"兵种名","兵种类型","造价","视野","移动力","移动类型","燃料"
};*/

Table_UnitsData::Table_UnitsData():source(nullptr){
	renderItemAmount=10;
	maxPage=2;
	/*for(int i=0;i<7;++i){
		stringCode_Headers[i]=GameString::newString(headerName[i]);
	}*/
}
Table_UnitsData::~Table_UnitsData(){}

uint Table_UnitsData::columnAmount()const{return 4;}
static uint colWidth[]={480,160,96,64,96,160,64};
uint Table_UnitsData::columnWidth(uint col)const{
	if(col==0)return colWidth[col];
	else{
		switch(page){
			case 0:return colWidth[col];
			case 1:return colWidth[col+3];
		}
	}
	return 0;
}
void Table_UnitsData::render()const{
	GameTable::render();
	if(!source)return;
	//画数据
	/*rect=rectF();
	gameString.position.y()=rect.p1.y()-itemHeight;
	uint index=0;
	uint renderTo=renderItemFrom+renderItemAmount;
	for(auto item:*source){
		if(index>=renderItemFrom && index < renderTo){//需要渲染的部分
			gameString.position.y()-=itemHeight;
			gameString.position.x()=rect.p0.x();
			//设置颜色
			if(index==selectingItem){//当前项,我们设置成白底黑字
				gameString.color=0xFF000000;//黑字
				sr.fillColor=color;//当前底
				sr.drawRectangle(
					rect.p0.x(),
					gameString.position.y(),
					rect.p1.x(),gameString.position.y()+itemHeight);
			}else{//非当前项,我们设置成白字
				gameString.color=0xFFFFFFFF;
			}
			//显示兵种图标
			if(textures){
				glColor4ub(255,255,255,255);
				auto tex=textures->value(index);
				if(tex)tex->draw(gameString.posF());
			}
			//兵种名
			gameString.position.x()+=32;
			gameString.setString(item.name);
			gameString.anchorPoint=Point2D<float>();
			gameString.render();
			gameString.position.x()-=32;
			switch(page){
				case 0:{
					//兵种类型
					gameString.setString(item.corpType);
					gameString.anchorPoint.x()=0.5;
					gameString.position.x()+=columnWidth(0);
					gameString.position.x()+=columnWidth(1)/2;
					gameString.render();
					gameString.position.x()+=columnWidth(1)/2;
					//造价
					char tmp[12];
					sprintf(tmp,"%d",item.price);
					gameString.setString(tmp);
					gameString.position.x()+=columnWidth(2)/2;
					gameString.render();
					gameString.position.x()+=columnWidth(2)/2;
					//视野
					sprintf(tmp,"%d",item.vision);
					gameString.setString(tmp);
					gameString.position.x()+=columnWidth(3)/2;
					gameString.render();
					gameString.position.x()+=columnWidth(3)/2;
				}break;
				case 1:{
					//移动力
					char tmp[12];
					sprintf(tmp,"%d",item.movement);
					gameString.setString(tmp);
					gameString.anchorPoint.x()=0.5;
					gameString.position.x()+=columnWidth(0);
					gameString.position.x()+=columnWidth(1)/2;
					gameString.render();
					gameString.position.x()+=columnWidth(1)/2;
					//移动类型
					gameString.setString(item.moveType);
					gameString.position.x()+=columnWidth(2)/2;
					gameString.render();
					gameString.position.x()+=columnWidth(2)/2;
					//燃料
					sprintf(tmp,"%d",item.gasMax);
					gameString.setString(tmp);
					gameString.position.x()+=columnWidth(3)/2;
					gameString.render();
					gameString.position.x()+=columnWidth(3)/2;
				}break;
			}
		}
		++index;//下一个
	}
	gameString.stringCode.memoryFree();
	gameString.stringCode=DataBlock();*/
}

DataBlock Table_UnitsData::headerBlock(uint col)const{
	/*if(stringCode_Headers){
		if(col>0 && page==1){
			col+=3;
		}
		return stringCode_Headers[col];
	}*/
	return DataBlock();
}
