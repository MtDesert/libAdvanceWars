#include"BattleField.h"
#include"define.h"

#include<stdlib.h>

BattleField::BattleField():corpsList(nullptr),troopsList(nullptr),terrainsList(nullptr),whenError(nullptr){}
BattleField::~BattleField(){}

bool BattleField::getTerrain(uint x,uint y,Terrain &terrain)const{return getValue(x,y,terrain);}
bool BattleField::getTerrain(const Point2D<int> &p,Terrain &terrain)const{return getTerrain(p.x,p.y,terrain);}
bool BattleField::setTerrain(uint x,uint y,const Terrain &terrain){return setValue(x,y,terrain);}
bool BattleField::setTerrain(const Point2D<int> &p,const Terrain &terrain){return setTerrain(p.x,p.y,terrain);}
bool BattleField::setTerrain(uint x,uint y,const string &terrainName,const string &status){
	Terrain terrain;
	SizeType trnIndex=0,trpIndex=0;
	if(terrainsList->dataName(terrainName,trnIndex)){
		terrain.terrainType=trnIndex;
		if(!status.empty()){
			troopsList->dataName(status,trpIndex);
		}
		terrain.status=trpIndex;
		printf("set(%u,%u,%u,%u)->%s,%s\n",x,y,terrain.terrainType,terrain.status,terrainName.data(),status.data());
		return setTerrain(x,y,terrain);
	}
	return false;
}
bool BattleField::addUnit(uint x,uint y,const string &corpName,const string &troopName){
	Unit unit;
	SizeType crpIndex=0,trpIndex=0;
	auto corp=corpsList->dataName(corpName,crpIndex);
	if(corp){
		unit.corpType=crpIndex;//兵种
		if(!troopName.empty() && troopsList->dataName(troopName,trpIndex)){
			unit.color=trpIndex;//势力
		}
		unit.coordinate=decltype(unit.coordinate)(x,y);//坐标
		unit.fuel=corp->gasMax;//燃料
		for(auto &wpn:corp->weapons){
			unit.ammunition=wpn.ammunitionMax;
			break;
		}
		chessPieces.push_back(unit);
	}
	return false;
}

bool BattleField::fillTerrain(const Terrain &terrain){
	for(unsigned x=0;x<width;++width){
		for(unsigned y=0;y<height;++y){
			setValue(x,y,terrain);
		}
	}
	return true;
}

void BattleField::autoAdjustTerrainsTiles(){
	if(!terrainsList)return;
	for(uint x=0;x<width;++x){
		for(uint y=0;y<height;++y){
			autoAdjustTerrainTile(x,y);
		}
	}
}
void BattleField::autoAdjustTerrainTile(uint x,uint y){
	if(!terrainsList)return;
	//获取当前地形
	Terrain terrain;
	if(!getTerrain(x,y,terrain))return;
	if(!terrainsList->canAdjustTile(terrain.terrainType))return;
	//生成四周的坐标
	List<Point2D<int> > pList;
	pList.push_back(Point2D<int>(x,y-1));
	pList.push_back(Point2D<int>(x,y+1));
	pList.push_back(Point2D<int>(x-1,y));
	pList.push_back(Point2D<int>(x+1,y));
	Terrain terrainX;
	//根据坐标判断连通性
	auto bit=1;
	for(auto &item:pList){
		if(getTerrain(item,terrainX)){
			if(terrainsList->canAdjustTile(terrain.terrainType,terrainX.terrainType)){
				terrain.status|=bit;
			}
		}else{//边界,可判定为连通
			terrain.status|=bit;
		}
		bit<<=1;
	}
	setTerrain(x,y,terrain);
}

int BattleField::loadMap(const string &filename){
	//根据扩展名来识别
	auto pos=filename.find_last_of(".");
	auto suffix=filename.substr(pos);
	auto len=suffix.length();
	if(len!=4)return -1;//目前高战地图文件的扩展名包括点共4个
	for(decltype(len) i=0;i<len;++i){//转换成小写再比对
		suffix[i]=tolower(suffix[i]);
	}
	//开始比对
	return -1;
}

//逗号分隔
static void commaSeperate(char *str,char* strAddr[],int &strAddrLen){
	int i=0;
	auto start=str;
	for(;i<strAddrLen;++i){
		if(!start || *start=='\0')break;
		auto fin=strchr(start,',');
		if(!fin)fin=strchr(start,'\n');
		if(fin)*fin='\0';//变成字符串
		//保存数据
		strAddr[i]=start;
		//下一个
		if(fin)start=fin+1;
		else start=nullptr;
	}
	strAddrLen=i;
}

bool BattleField::loadMap_CSV(const string &filename){
	ASSERT(corpsList,"No corps list")
	ASSERT(troopsList,"No troops list")
	ASSERT(terrainsList,"No terrains list")
	//打开文件
	FILE *file=fopen(filename.data(),"rb");
	ASSERT_ERRNO(file)
	//开始读取地图名和作者
	char buffer[BUFSIZ];
	mapName=fgets(buffer,BUFSIZ,file);
	mapName.pop_back();//因为自带了换行符'\n'
	author=fgets(buffer,BUFSIZ,file);
	author.pop_back();//因为自带了换行符'\n'
	//读取地图尺寸
	int w,h;
	if(fgets(buffer,BUFSIZ,file) && sscanf(buffer,"%d,%d",&w,&h)==2){
		newData(w,h);//申请内存
		for(int y=0;y<h;++y){
			if(fgets(buffer,BUFSIZ,file)){//逐行处理
				char* strAddr[w];//缓存地形名称
				int total=w;//实际数量(有可能读到的数量不为w)
				commaSeperate(buffer,strAddr,total);
				for(int x=0;x<total;++x){
					auto name=strAddr[x];//地形名
					auto dot=strchr(name,'.');//寻找分隔符
					if(dot)*dot='\0';//变成字符串
					auto trpName=(dot ? dot+1 : "");//势力的名字可能有
					//解析地形
					setTerrain(x,y,name,trpName);
				}
			}
		}
	}
	//自动调整地形
	autoAdjustTerrainsTiles();
	//读取作战单位
	chessPieces.clear();
	char* strAddr[4];//x,y,兵种名,部队名
	int x,y;
	while(fgets(buffer,BUFSIZ,file)){
		//分析数据量
		int total=4;
		commaSeperate(buffer,strAddr,total);
		if(total!=4)continue;
		//读取数据
		if(sscanf(strAddr[0],"%d",&x)==1 && sscanf(strAddr[1],"%d",&y)==1){
			addUnit(x,y,strAddr[2],strAddr[3]);
		}
	}
	//关闭文件
	fclose(file);
	return true;
}

int BattleField::saveMap_CSV(const string &filename)const{
	if(!corpsList || !troopsList || !terrainsList)return -1;
	//写LUA文件
	FILE *file=fopen(filename.data(),"wb");
	if(!file)return -1;
	saveMap_CSV(file);
	//写入完毕
	fflush(file);
	fclose(file);
	return 0;
}

void BattleField::saveMap_CSV(FILE *file)const{
	//基本信息
	int w=getWidth();
	int h=getHeight();
	fprintf(file,"%s\n",mapName.data());
	fprintf(file,"%s\n",author.data());
	fprintf(file,"%d,%d\n",w,h);
	//打印地形信息
	Terrain terrain;
	for(decltype(h) y=0;y<h;++y){
		for(decltype(w) x=0;x<w;++x){
			if(getTerrain(x,y,terrain)){
				auto code=terrainsList->data(terrain.terrainType);
				if(!code)continue;
				if(code->capturable){
					auto troop=troopsList->data(terrain.status);
					if(troop){
						fprintf(file,"%s.%s",code->name.data(),troop->name.data());
					}else{
						fprintf(file,"%s",code->name.data());
					}
				}else{
					fprintf(file,"%s",code->name.data());
				}
				//打印逗号或者换行
				if(x+1<w){
					fprintf(file,",");
				}else if(y+1<h){
					fprintf(file,"\n");
				}
			}
		}
	}
	//打印单位信息
	for(auto &unit:chessPieces){
		auto x=unit.coordinate.x;
		auto y=unit.coordinate.y;
		auto code=corpsList->data(unit.corpType);//查兵种表以确认兵种
		if(!code)continue;
		fprintf(file,"\n");
		auto troop=troopsList->data(unit.color);//查部队表以确认据点所属
		if(troop){
			fprintf(file,"%d,%d,%s,%s",x,y,code->name.data(),troop->name.data());
		}else{
			fprintf(file,"%d,%d,%s",x,y,code->name.data());
		}
	}
}

//static bool func(const int &a,const int &b){return a<b;}
void BattleField::analyse(){
	//清除原来的分析结果
	onlySea=true;
	captureTerrainCount=0;
	playerIndexList.clear();
	//准备分析
	int w=getWidth();
	int h=getHeight();
	Terrain terrain;
	for(decltype(h) y=0;y<h;++y){
		for(decltype(w) x=0;x<w;++x){
			if(getTerrain(x,y,terrain)){
				//开始统计可能的玩家个数
				auto code=terrainsList->data(terrain.terrainType);
				if(!code)continue;
				if(code->name!="Sea")onlySea=false;
				if(code->capturable){
					auto troop=troopsList->data(terrain.status);
					if(troop && troop->name!="Neutral"){
						playerIndexList.insert(terrain.status);
					}
					++captureTerrainCount;//累加据点数
				}
			}
		}
	}
	for(auto &unit:chessPieces){
		auto code=corpsList->data(unit.corpType);//查兵种表以确认兵种
		if(!code)continue;
		auto troop=troopsList->data(unit.color);//查部队表以确认据点所属
		if(troop && troop->name!="Neutral"){
			playerIndexList.insert(unit.color);
		}
	}
	//然后我们需要将玩家序号排序
	//playerIndexList.sort(func);
}