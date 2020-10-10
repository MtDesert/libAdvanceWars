#include"BattleField.h"
#include"define.h"
#include"Map.h"

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
		auto idx=chessPieces.indexOf([&](const Unit &u){//查询是否需要覆盖旧的数据
			return u.coordinate==unit.coordinate;
		});
		if(idx>=0){
			chessPieces.setData(idx,unit);
			trn->unitIndex=idx;
		}else{
			chessPieces.push_back(unit);
			trn->unitIndex=chessPieces.size()-1;
		}
		return chessPieces.data(trn->unitIndex);
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
	if(trn){
		trn->unitIndex=TERRAIN_NO_UNIT;//从地形上消失
		return true;
	}
	return false;
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

void BattleField::forEachUnit(function<void(Unit &unit)> callback){
	Terrain *trn=nullptr;
	chessPieces.forEach([&](Unit &unit,SizeType index){
		trn=getTerrain(unit.coordinate);
		if(!trn)return;
		//判断单位有没有从战场上消失
		if(trn->unitIndex!=index)return;//消失了
		//只处理还没消失的部队
		callback(unit);
	});
}

void BattleField::fillTerrain(const Terrain &terrain){
	forEachLattice([&](uint x,uint y,Terrain &terrain){
		setValue(x,y,terrain);
	});
}

void BattleField::autoAdjustTerrainsTiles(){
	if(!terrainsList)return;
	forEachLattice([&](uint x,uint y,Terrain &terrain){
		autoAdjustTerrainTile(x,y);
	});
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

static const char* encodeString="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const SizeType encodeLen=strlen(encodeString);
static void printTerrainCode(FILE *file,SizeType index){
	if(index<encodeLen)fprintf(file,"%c",encodeString[index]);//单个数字或字母表示
	else{//否则用两个字母表示
		index-=encodeLen;//从"00"开始编码到"zz"
		fprintf(file,"%c%c",encodeString[index/encodeLen],encodeString[index%encodeLen]);
	}
}
static int getTerrainIndex(const char* name){
	int ret=0;
	auto nameLen=strlen(name);
	for(size_t i=0;i<nameLen;++i){
		auto ptr=strchr(encodeString,name[i]);
		if(ptr){//计算每一位,不断更新返回值
			ret*=encodeLen;
			ret+=(ptr-encodeString);
		}else{//出错了
			return -1;
		}
	}
	//开始分析值
	return ret;
}
static int getTerrainIndex(char ch){
	char chr[2]={0};
	chr[0]=ch;//变成字符串
	return getTerrainIndex(chr);
}

static char* charRet=nullptr;
#define READ_LINE fgets(buffer,BUFSIZ,file)

bool BattleField::loadMap_CSV(const string &filename){
	ASSERT(corpsList,"No corps list")
	ASSERT(troopsList,"No troops list")
	ASSERT(terrainsList,"No terrains list")
	mapFilename=filename;
	//打开文件
	FILE *file=fopen(filename.data(),"rb");
	//开始读取地图名和作者
	char buffer[BUFSIZ];
	mapName=READ_LINE;
	mapName.pop_back();//因为自带了换行符'\n'
	author=READ_LINE;
	author.pop_back();//因为自带了换行符'\n'
	//读取地图尺寸
	int w,h;
	ASSERT(READ_LINE && sscanf(buffer,"%d,%d",&w,&h)==2,"Size format error")
	//读取地形表
	charRet=READ_LINE;//读取空行
	Array<Terrain> terrainArray;
	Terrain terrain;
	SizeType terrainID,troopID;
	while(READ_LINE){
		//去掉行尾换行符号
		buffer[strlen(buffer)-1]='\0';
		if(buffer[0]=='\0')break;//读取结束
		//分析格式
		auto troopName=strchr(buffer,'.');//直接分析'.',可能拿到部队名
		if(troopName){//构造成字符串以便读取
			*troopName='\0';
			++troopName;
		}
		//开始读取
		if(terrainsList->dataName(buffer,terrainID)){
			//获取terrainID成功,再确定troopID
			troopID=0;
			if(troopName)troopsList->dataName(troopName,troopID);
			//插入Terrain
			terrain.terrainType=terrainID;
			terrain.status=troopID;
			terrainArray.push_back(terrain);
		}
	}
	//读取地形数据
	bool needComma = terrainArray.size()>encodeLen;
	newData(w,h);//申请内存
	for(int y=h-1;y>=0;--y){
		if(READ_LINE){//逐行处理
			if(needComma){//把','转化成'\0'以便读取
				auto len=strlen(buffer);
				for(size_t i=0;i<len;++i){
					if(buffer[i]==',')buffer[i]='\0';
				}
			}
			//开始读取
			for(int x=0;x<w;++x){
				if(needComma){
				}else{//开始对照码表
					auto idx=getTerrainIndex(buffer[x]);
					auto trn=terrainArray.data(idx);
					if(trn){
						setTerrain(x,y,*trn);
					}
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
	//构造地形表
	BattleField_Feature feature;
	analyseFeature(feature);
	Terrain terrain;
	Array<Terrain> terrainArray;//借用Terrain的结构,用unitIndex来表示统计数量
	feature.array_terrainAmount.forEach([&](SizeType x,SizeType y,SizeType &value){
		if(value>0){
			terrain.terrainType=x;
			terrain.status=y;
			terrain.unitIndex=value;
			terrainArray.push_back(terrain);
		}
	});
	//地形表简单排序
	terrainArray.sort([](const Terrain &a,const Terrain &b){
		if(a.terrainType==b.terrainType)return a.status<b.status;
		else return a.terrainType<b.terrainType;
	});
	//输出地形表
	terrainArray.forEach([&](const Terrain &terrain){
		auto code=terrainsList->data(terrain.terrainType);
		if(!code)return;
		fprintf(file,"\n");
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
	});
	fprintf(file,"\n");
	//输出地形信息
	bool needComma = terrainArray.size()>encodeLen;
	forEachLattice([&](SizeType x,SizeType y,const Terrain &terrain){
		//打印逗号或者换行
		fprintf(file,x>0?(needComma?",":""):"\n");
		auto index=terrainArray.indexOf([&](const Terrain &trn){
			if(terrain.terrainType==trn.terrainType){
				auto code=terrainsList->data(trn.terrainType);
				return code->capturable ? terrain.status==trn.status : true;
			}
			return false;
		});
		printTerrainCode(file,index);
	});
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
	feature.array_terrainAmount.newData(terrainsList->size(),troopsList->size(),0);
	feature.array_buildableTerrainAmount.setSize(troopsList->size(),true);
	feature.array_buildableTerrainAmount.fill(0);
	feature.array_UnitAmount.setSize(troopsList->size(),true);
	feature.array_UnitAmount.fill(0);

	//开始遍历各个地形
	TerrainCode *code=nullptr;
	SizeType status=0,amount=0;
	forEachLattice([&](uint x,uint y,const Terrain &terrain){
		//获取对应地形码
		code=terrainsList->data(terrain.terrainType);
		if(!code)return;
		//统计各个地形数量
		status = code->capturable ? terrain.status : 0;//若为据点,则需要status来区分势力
		if(feature.array_terrainAmount.getValue(terrain.terrainType,status,amount)){
			++amount;
			feature.array_terrainAmount.setValue(terrain.terrainType,status,amount);
		}
		//统计生产据点数
		if(!code->produceType.empty()){
			auto pNum=feature.array_buildableTerrainAmount.data(terrain.status);
			if(pNum)++(*pNum);
		}
	});
	//统计各个势力的单位数
	for(auto &unit:chessPieces){
		auto pNum=feature.array_UnitAmount.data(unit.color);
		if(pNum)++(*pNum);
	}
}