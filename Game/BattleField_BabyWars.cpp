#include"BattleField.h"
//转化成BabyWars可用地图的模块

//地形码表
static int codeTable_River[]={2,6,5,12,4,11,9,16,3,10,8,15,7,14,13,17};
static int codeTable_Sea[]={64,51,60,42,62,47,56,34,63,49,58,38,61,43,52,18};
static int codeTable_Shoal[]={98,98,97,99,100,95,94,68,99,96,93,67,98,66,65,97};
static int codeTable_TileBase(const string &name,uint8 terrainStatus){
	if(name=="Plain"){
		return 1;
	}else if(name=="River"){
		return codeTable_River[terrainStatus];
	}else if(name=="Sea"){//桥下也先默认放海
		return codeTable_Sea[terrainStatus];
	}else if(name=="Shoal"){
		return codeTable_Shoal[terrainStatus];
	}
	return 1;
}
static int codeTable_Road[]={111,102,102,102,101,106,104,110,101,105,103,109,101,108,107,111};
static int codeTable_Bridge[]={122,113,113,113,112,117,115,121,112,116,114,120,112,119,118,122};
static int codeTable_Plasma[]={131,132,134,146,133,142,143,137,135,141,140,139,145,136,138,144};

//根据地形和状态获得地形码
static int codeTable_TileObject(const string &name,int status){
	if(name=="Road"){
		return codeTable_Road[status];
	}else if(name=="Bridge"){
		return codeTable_Bridge[status];
	}else if(name=="Wood"){
		return 123;
	}else if(name=="Mountain"){
		return 124;
	}else if(name=="WasteLand"){
		return 125;
	}else if(name=="Ruin"){
		return 126;
	}else if(name=="Reef"){
		return 130;
	}else if(name=="Pipe"){
		return codeTable_Plasma[status];
	}else if(name=="PipeSeam"){
		return 147;
	}else if(name=="Silo"){
		return 148;
	}else if(name=="SiloEmpty"){
		return 149;
	}else if(name=="HQ"){
		return 150+status;
	}else if(name=="City"){
		return 154+status;
	}else if(name=="ComTower"){
		return 159+status;
	}else if(name=="Lab" || name=="Radar"){
		return 164+status;
	}else if(name=="Factory"){
		return 169+status;
	}else if(name=="AirFactory"){
		return 174+status;
	}else if(name=="ShipFactory"){
		return 179+status;
	}else if(name=="AirPort"){
		return 184+status;
	}else if(name=="ShipPort"){
		return 189+status;
	}
	return 0;
}

//根据单位和状态名字获得地形码
static int codeTable_unit(const string &name,int status){
	if(name=="Infantry"){
		return 210+status;
	}else if(name=="Mech"){
		return 214+status;
	}else if(name=="Recon"){
		return 222+status;
	}else if(name=="AntiAir"){
		return 230+status;
	}else if(name=="Tank"){
		return 234+status;
	}else if(name=="MiddleTank"){
		return 238+status;
	}else if(name=="Artillery"){
		return 246+status;
	}else if(name=="Rockets"){
		return 254+status;
	}else if(name=="Missiles"){
		return 258+status;
	}else if(name=="APC"){
		return 262+status;
	}else if(name=="Fighter"){
		return 266+status;
	}else if(name=="Bomber"){
		return 270+status;
	}else if(name=="BattleCopter"){
		return 278+status;
	}else if(name=="TransportCopter"){
		return 282+status;
	}else if(name=="BattleShip"){
		return 290+status;
	}else if(name=="Carrier"){
		return 294+status;
	}else if(name=="Submarine"){
		return 298+status;
	}else if(name=="Cruiser"){
		return 302+status;
	}else if(name=="Lander"){
		return 306+status;
	}else if(name=="BlackBomb"){//转换成炮艇
		return 310+status;
	}
	return 0;
}

//检测能否输出
static FILE* babyWars_OutputFile(const BattleField &bf,const string &filename){
	if(!bf.corpsList || !bf.troopsList || !bf.terrainsList)return nullptr;//表不全,无法转换
	auto playersCount=bf.playerIndexList.size();
	if(playersCount<2 || playersCount>4)return nullptr;//玩家数超出BabyWars范围,不转换
	return fopen(filename.data(),"wb");
}

//输出lua的地图头
static void babyWars_WriteHeader_lua(const BattleField &bf,FILE *file){
	//文件头基本信息
	fprintf(file,"return{\n");
	fprintf(file,"\twarFieldName=\'%s\',\n",bf.mapName.data());
	fprintf(file,"\tauthorName=\'%s\',\n",bf.author.data());
	fprintf(file,"\tplayersCount=%lu,\n",bf.playerIndexList.size());
	fprintf(file,"\twidth=%d,\n",bf.getWidth());
	fprintf(file,"\theight=%d,\n",bf.getHeight());
	fprintf(file,"\tlayers={\n");
}

//输出tmx的地图头
static bool babyWars_WriteHeader_tmx(const BattleField &bf,FILE *file){
	//打开模板头文件
	auto headerFile=fopen("tools/template.tmx","rb");
	if(!headerFile)return false;
	//开始复制
	char buffer[BUFSIZ];
	int read=0;
	do{
		read=fread(buffer,1,BUFSIZ,headerFile);
		if(read){
			fwrite(buffer,1,read,file);
		}
	}while(read);
	//复制结束
	fclose(headerFile);
	return true;
}

static int *layer_TileBase=nullptr;
static int *layer_TileObject=nullptr;
static int *layer_unit=nullptr;
//生成图层数据块
static void makeLayerData(const BattleField &bf){
	//初始化图层数据块
	int w=bf.getWidth(),h=bf.getHeight();
	int wh=w*h;
	layer_TileBase=new int[wh];
	layer_TileObject=new int[wh];
	layer_unit=new int[wh];
	for(int i=0;i<wh;++i){
		layer_TileBase[i]=1;
		layer_TileObject[i]=0;
		layer_unit[i]=0;
	}
	//开始转化地形
	auto playersCount=bf.playerIndexList.size();
	Terrain terrain;
	for(decltype(h) y=0;y<h;++y){
		for(decltype(w) x=0;x<w;++x){
			if(bf.getTerrain(x,y,terrain)){
				auto code=bf.terrainsList->data(terrain.terrainType);
				if(!code)continue;
				//得到地形信息,可以转换了
				layer_TileBase[y*w+x]=codeTable_TileBase(code->name,terrain.status);
				if(code->capturable){//据点要特殊处理
					uint8 status=0;
					for(auto &index:bf.playerIndexList){
						if(index==terrain.status)break;
						++status;
					}
					if(status>=playersCount)status=0;//中立据点
					else if(code->name!="HQ")++status;//总部没有中立的
					layer_TileObject[y*w+x]=codeTable_TileObject(code->name,status);
				}else{
					layer_TileObject[y*w+x]=codeTable_TileObject(code->name,terrain.status);
					if(code->name=="Bridge"){//桥下地形要特殊处理
						//生成四周的坐标
						List<Point2D<int> > pList;
						pList.push_back(Point2D<int>(x,y-1));
						pList.push_back(Point2D<int>(x,y+1));
						pList.push_back(Point2D<int>(x-1,y));
						pList.push_back(Point2D<int>(x+1,y));
						Terrain trn;
						auto bit=1,status=0;
						bool hasRiver=false;
						for(auto &item:pList){
							if(bf.getTerrain(item,trn)){
								auto cd=bf.terrainsList->data(trn.terrainType);
								if(!cd)continue;
								if(cd->name=="River"){//周边有河
									hasRiver=true;
									break;//直接跳出循环
								}else if(cd->name=="Bridge"||cd->tileType=="Water"){
									status|=bit;
								}
							}else{//边界,可判定为连通
								status|=bit;
							}
							bit<<=1;
						}
						//周边有河,那么底就是河
						if(hasRiver){
							layer_TileBase[y*w+x]=codeTable_TileBase("River",15-terrain.status);
						}else{
							layer_TileBase[y*w+x]=codeTable_TileBase("Sea",status);
						}
					}
				}
			}
		}
	}
	//开始转化单位
	for(auto &unit:bf.chessPieces){
		auto code=bf.corpsList->data(unit.corpType);//查兵种表以确认兵种
		if(!code)continue;
		auto x=unit.coordinate.x;
		auto y=unit.coordinate.y;
		uint8 status=0;
		for(auto &index:bf.playerIndexList){
			if(index==unit.color)break;
			++status;
		}
		layer_unit[y*w+x]=codeTable_unit(code->name,status);
	}
}

//输出csv格式数据
void printCSV(FILE *file,int w,int h,int layerData[]){
	int wh=w*h;
	for(int i=0;i<wh;++i){
		fprintf(file,"%d",layerData[i]);
		if(i+1<wh)fprintf(file,",");
		if((i+1)%w==0)fprintf(file,"\n");
	}
}

//输出lua数据的layer数据
static void printLayer_lua(FILE *file,const string &name,int w,int h,int layerData[]){
	fprintf(file,"{type=\'tilelayer\',\n");
	fprintf(file,"name=\'%s\',\n",name.data());
	fprintf(file,"x=0,y=0,width=%d,height=%d,\n",w,h);
	fprintf(file,"visible=true,opacity=1,offsetx=0,offsety=0,\n");
	fprintf(file,"properties={},encoding=\'lua\',data={\n");
	printCSV(file,w,h,layerData);
	fprintf(file,"}}\n");
}

//输出lua数据
static void babyWars_WriteData_lua(const BattleField &bf,FILE *file){
	int w=bf.getWidth(),h=bf.getHeight();
	printLayer_lua(file,"TileBase",w,h,layer_TileBase);
	fprintf(file,",\n");
	printLayer_lua(file,"TileObject",w,h,layer_TileObject);
	fprintf(file,",\n");
	printLayer_lua(file,"unit",w,h,layer_unit);
	fprintf(file,"}}");
}

//输出tmx的layer数据
static void printLayer_tmx(FILE *file,const string &name,int w,int h,int layerData[]){
	fprintf(file,"<layer name=\"%s\" width=\"%d\" height=\"%d\">\n",name.data(),w,h);
	fprintf(file,"<data encoding=\"csv\">\n");
	printCSV(file,w,h,layerData);
	fprintf(file,"</data></layer>\n");
}

//输出tmx数据
static void babyWars_WriteData_tmx(const BattleField &bf,FILE *file){
	int w=bf.getWidth(),h=bf.getHeight();
	printLayer_tmx(file,"tileBase",w,h,layer_TileBase);
	printLayer_tmx(file,"tileObject",w,h,layer_TileObject);
	printLayer_tmx(file,"unit",w,h,layer_unit);
}

//结束输出,做收尾工作
static void babyWars_finishOutput(FILE *file){
	delete []layer_TileBase;
	delete []layer_TileObject;
	delete []layer_unit;
	fflush(file);
	fclose(file);
}

int BattleField::saveMap_LUA_BabyWars(const string &filename)const{
	//打开文件准备写
	auto *file=babyWars_OutputFile(*this,filename);
	if(!file)return -1;
	//开始写文件
	babyWars_WriteHeader_lua(*this,file);
	makeLayerData(*this);
	babyWars_WriteData_lua(*this,file);
	//写入完毕
	babyWars_finishOutput(file);
	return 0;
}
int BattleField::saveMap_TMX_BabyWars(const string &filename)const{
	//打开文件准备写
	auto *file=babyWars_OutputFile(*this,filename);
	if(!file)return -1;
	//开始写文件
	babyWars_WriteHeader_tmx(*this,file);
	makeLayerData(*this);
	babyWars_WriteData_tmx(*this,file);
	//写入完毕
	babyWars_finishOutput(file);
	return 0;
}
