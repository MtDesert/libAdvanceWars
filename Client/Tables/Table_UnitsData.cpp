#include"Table_UnitsData.h"
#include"GameString.h"
#include"ShapeRenderer.h"

#include"Charset.h"

Table_UnitsData::Table_UnitsData():source(nullptr){maxPage=2;}
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
}

DataBlock Table_UnitsData::headerBlock(uint col)const{return DataBlock();}
