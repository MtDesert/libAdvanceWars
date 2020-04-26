#include"BattleField.h"
#include"define.h"

void BattleField_Feature::print()const{
	printf("build:\n");
	for(SizeType i=0;i<array_buildableTerrainAmount.size();++i){
		printf("%lu -> %lu\n",i,*array_buildableTerrainAmount.data(i));
	}
	printf("unit:\n");
	for(SizeType i=0;i<array_UnitAmount.size();++i){
		printf("%lu -> %lu\n",i,*array_UnitAmount.data(i));
	}
}

BattleField::BattleField():corpsList(nullptr),troopsList(nullptr),terrainsList(nullptr),whenError(nullptr){}
BattleField::~BattleField(){}

bool BattleField::getTerrain(SizeType x,SizeType y,Terrain &terrain)const{return getValue(x,y,terrain);}
bool BattleField::getTerrain(const CoordType &p,Terrain &terrain)const{return getTerrain(p.x,p.y,terrain);}
Terrain* BattleField::getTerrain(SizeType x,SizeType y)const{return pointer(x,y);}
Terrain* BattleField::getTerrain(const CoordType &p)const{return pointer(p.x,p.y);}

bool BattleField::setTerrain(SizeType x,SizeType y,const Terrain &terrain){return setValue(x,y,terrain);}
bool BattleField::setTerrain(const CoordType &p,const Terrain &terrain){return setTerrain(p.x,p.y,terrain);}
bool BattleField::setTerrain(SizeType x,SizeType y,const string &terrainName,const string &status){
	auto trn=getTerrain(x,y);
	if(!trn)return false;
	SizeType trnIndex=trn->terrainType,trpIndex=trn->status;
	if(terrainsList->dataName(terrainName,trnIndex)){
		trn->terrainType=trnIndex;
		if(!status.empty()){
			troopsList->dataName(status,trpIndex);
		}
		trn->status=trpIndex;
		return true;
	}
	return false;
}
Unit* BattleField::addUnit(SizeType x,SizeType y,const string &corpName,const string &troopName){
	SizeType crpIndex=0,trpIndex=0;
	auto corp=corpsList->dataName(corpName,crpIndex);
	if(corp){
		if(!troopName.empty() && troopsList->dataName(troopName,trpIndex)){
			return addUnit(x,y,crpIndex,trpIndex);
		}
	}
	return nullptr;
}
Unit* BattleField::addUnit(SizeType x,SizeType y,SizeType corpID,SizeType troopID){
	Unit unit(corpID,troopID,decltype(unit.coordinate)(x,y));
	auto corp=corpsList->data(corpID);
	if(corp){
		unit.fuel=corp->gasMax;//燃料
		auto wpn=corp->weapons.data(0);//武器
		unit.ammunition = wpn ? wpn->ammunitionMax : 0;//弹药
		//添加
		return addUnit(unit);
	}
	return nullptr;
}
Unit* BattleField::addUnit(const Unit &unit){
	auto trn=getTerrain(unit.coordinate);
	if(trn){
		chessPieces.push_back(unit);
		trn->unitIndex=chessPieces.size()-1;
		return chessPieces.lastData();
	}
	return nullptr;
}

bool BattleField::removeUnit(const CoordType &p){
	for(auto &unit:chessPieces){
		if(unit.coordinate==p)return removeUnit(unit);
	}
	return false;
}
bool BattleField::removeUnit(SizeType x,SizeType y){return removeUnit(CoordType(x,y));}
bool BattleField::removeUnit(const Unit &unit){
	//地形关系中移除
	auto trn=getTerrain(unit.coordinate);
	if(trn)trn->unitIndex=TERRAIN_NO_UNIT;
	//队列中移除
	auto sz=chessPieces.size();
	chessPieces.remove(&unit);//队列中移除
	return sz!=chessPieces.size();
}

Unit* BattleField::getUnit(SizeType x,SizeType y)const{
	auto trn=getTerrain(x,y);
	return trn ? getUnit(*trn) : nullptr;
}
Unit* BattleField::getUnit(const CoordType &p)const{
	auto trn=getTerrain(p);
	return trn ? getUnit(*trn) : nullptr;
}
Unit* BattleField::getUnit(const Terrain &terrain)const{return chessPieces.data(terrain.unitIndex);}

bool BattleField::fillTerrain(const Terrain &terrain){
	for(SizeType x=0;x<width;++x){
		for(SizeType y=0;y<height;++y){
			setValue(x,y,terrain);
		}
	}
	return true;
}

void BattleField::autoAdjustTerrainsTiles(){
	if(!terrainsList)return;
	for(SizeType x=0;x<width;++x){
		for(SizeType y=0;y<height;++y){
			autoAdjustTerrainTile(x,y);
		}
	}
}

void BattleField::autoAdjustTerrainTile(SizeType x,SizeType y,bool adjustAround){
	if(!terrainsList)return;
	//获取当前地形
	Terrain terrain;
	if(!getTerrain(x,y,terrain))return;
	auto code=terrainsList->data(terrain.terrainType);
	if(!code)return;
	//生成四周的坐标
	CoordType direction4[4];//autoAdjustTerrainTile专用,用于计算周围的4个点
	direction4[0].setXY(x,y+1);
	direction4[1].setXY(x,y-1);
	direction4[2].setXY(x-1,y);
	direction4[3].setXY(x+1,y);
	//根据坐标判断连通性
	if(!code->capturable)terrain.status=0;//非据点,需要重新计算status
	Terrain terrainX;
	auto bit=1;
	for(int i=0;i<4;++i){
		//调整图块
		auto p=direction4[i];
		if(code->has4direction){
			if(getTerrain(p,terrainX)){
				if(terrainsList->canAdjustTile(terrain.terrainType,terrainX.terrainType)){
					terrain.status|=bit;
				}
			}else{//边界,可判定为连通
				terrain.status|=bit;
			}
			bit<<=1;
		}
		//调整相邻图块
		if(adjustAround){
			autoAdjustTerrainTile(p.x,p.y);
		}
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
		for(int y=h-1;y>=0;--y){
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
	for(int y=h-1;y>=0;--y){
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
				}else if(y>0){
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

void BattleField::analyseFeature(BattleField_Feature &feature)const{
	//初始化
	feature.array_buildableTerrainAmount.setSize(troopsList->size(),true);
	feature.array_buildableTerrainAmount.fill(0);
	feature.array_UnitAmount.setSize(troopsList->size(),true);
	feature.array_UnitAmount.fill(0);
	//开始遍历各个地形
	auto w=getWidth();
	auto h=getHeight();
	Terrain terrain;
	for(decltype(h) y=0;y<h;++y){
		for(decltype(w) x=0;x<w;++x){
			if(getTerrain(x,y,terrain)){
				//开始统计可能的玩家个数
				auto code=terrainsList->data(terrain.terrainType);
				if(!code)continue;
				//统计生产据点数
				if(!code->produceType.empty()){
					auto pNum=feature.array_buildableTerrainAmount.data(terrain.status);
					if(pNum)++(*pNum);
				}
			}
		}
	}
	for(auto &unit:chessPieces){
		auto pNum=feature.array_UnitAmount.data(unit.color);
		if(pNum)++(*pNum);
	}
}