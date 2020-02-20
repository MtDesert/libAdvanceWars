#include"Table_CommandersData.h"

Table_CommandersData::Table_CommandersData(){
	//必须先创建缓冲区
	uint amount=12;
	itemBuffers=new ItemBuffer[amount];
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