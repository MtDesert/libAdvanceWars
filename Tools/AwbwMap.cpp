#include"AwbwMap.h"
#include"CurlEasy.h"
#include"typedef.h"

#include<list>

#include<stdio.h>
#include<string.h>

//据点码表
static int cityID[]={34,38,43,48,53,81,86,91,96,119,124,151,158,165,172,183,190};
static int factoryID[]={35,39,44,49,54,82,87,92,97,118,123,150,157,164,171,182,189};
static int airportID[]={36,40,45,50,55,83,88,93,98,117,122,149,156,163,170,181,188};
static int shiportID[]={37,41,46,51,56,84,89,94,99,121,126,155,162,169,176,187,194};
static int hqID[]={42,47,52,57,85,90,95,100,120,125,153,160,167,174,185,192};
static int comtowerID[]={127,128,129,130,131,132,133,134,135,136,137,152,159,166,173,184,191};
static int labID[]={138,139,140,141,142,143,144,145,146,147,148,154,161,168,175,186,193};
//势力码表
static int orangeStarID[]={38,39,40,41,42,134,146};
static int blueMoonID[]={43,44,45,46,47,129,140};
static int greenEarthID[]={48,49,50,51,52,131,142};
static int yellowCometID[]={53,54,55,56,57,136,148};
static int blackHoleID[]={91,92,93,94,95,128,139};
static int redFireID[]={81,82,83,84,85,135,147};
static int greySkyID[]={86,87,88,89,90,137,143};
static int brownDesertID[]={96,97,98,99,100,130,141};
static int amberBlazeID[]={117,118,119,120,121,127,138};
static int jadeSunID[]={122,123,124,125,126,132,144};
static int cobaltIceID[]={149,150,151,152,153,154,155};
static int pinkCosmosID[]={153,157,158,159,160,161,162};
static int tealGalaxyID[]={163,164,165,166,167,168,169};
static int purpleLightningID[]={170,171,172,173,174,175,176};
static int acidRainID[]={181,182,183,184,185,186,187};
static int whiteNovaID[]={188,189,190,191,192,193,194};

//名字映射
struct NameMap{
	const char *shortName,*name;
};
//势力名称映射表
static NameMap troopNames[]={
	{"os","OrangeStar"},
	{"bm","BlueMoon"},
	{"ge","GreenEarth"},
	{"yc","YellowComet"},
	{"bh","BlackHole"},
	{"rf","RedFire"},
	{"gs","GreySky"},
	{"bd","BrownDesert"},
	{"ab","AmberBlaze"},
	{"js","JadeSun"},
	{"ci","CobaltIce"},
	{"pc","PinkCosmos"},
	{"tg","TealGalaxy"},
	{"pl","PurpleLightning"},
	{"ar","AcidRain"},
	{"wn","WhiteNova"},
};
//兵种名称映射表
static NameMap corpNames[]={
	{"infantry","Infantry"},
	{"mech","Mech"},
	{"apc","APC"},
	{"recon","Recon"},
	{"rocket","Rockets"},
	{"missile","Missiles"},
	{"artillery","Artillery"},
	{"anti-air","AntiAir"},
	{"tank","Tank"},
	{"mdtank","MiddleTank"},
	{"neotank","NeoTank"},
	{"megatank","MegaTank"},
	{"piperunner","PipeRunner"},
	{"t-copter","TransportCopter"},
	{"b-copter","BattleCopter"},
	{"fighter","Fighter"},
	{"bomber","Bomber"},
	{"stealth","Stealth"},
	{"blackbomb","BlackBomb"},
	{"lander","Lander"},
	{"battleship","BattleShip"},
	{"cruiser","Cruiser"},
	{"sub","SubMarine"},
	{"carrier","Carrier"},
	{"blackboat","BlackBoat"}
};

//根据id查询图块名字
const char* strTileName(int id){
	if(id==1){return "Plain";
	}else if(id==2){return "Mountain";
	}else if(id==3){return "Wood";
	}else if(id>=4&&id<=14){return "River";
	}else if(id>=15&&id<=25){return "Road";
	}else if(id>=26&&id<=27){return "Bridge";
	}else if(id==28){return "Sea";
	}else if(id>=29&&id<=32){return "Shoal";
	}else if(id==33){return "Reef";
	}else if(id>=101&&id<=110){return "Pipe";
	}else if(id==111){return "Silo";
	}else if(id==112){return "SiloEmpty";
	}else if(id>=113&&id<=114){return "PipeSeam";
	}else if(id>=115&&id<=116){return "PipeRubble";
	}else{//查据点表
		int size=0;
#define STRTILENAME(array,name)\
		size=sizeof(array)/sizeof(int);\
		for(int i=0;i<size;++i){if(array[i]==id)return #name;}
		STRTILENAME(cityID,City)
		STRTILENAME(factoryID,Factory)
		STRTILENAME(airportID,AirFactory)
		STRTILENAME(shiportID,ShipFactory)
		STRTILENAME(hqID,HQ)
		STRTILENAME(comtowerID,ComTower)
		STRTILENAME(labID,Lab)
	}
	return nullptr;
}
//根据id查询据点名字
const char* strTroopName(int id){
	int size=0;
	STRTILENAME(orangeStarID,OrangeStar)
	STRTILENAME(blueMoonID,BlueMoon)
	STRTILENAME(greenEarthID,GreenEarth)
	STRTILENAME(yellowCometID,YellowComet)
	STRTILENAME(blackHoleID,BlackHole)
	STRTILENAME(redFireID,RedFire)
	STRTILENAME(greySkyID,GreySky)
	STRTILENAME(brownDesertID,BrownDesert)
	STRTILENAME(amberBlazeID,AmberBlaze)
	STRTILENAME(jadeSunID,JadeSun)
	STRTILENAME(cobaltIceID,CobaltIce)
	STRTILENAME(pinkCosmosID,PinkCosmos)
	STRTILENAME(tealGalaxyID,TealGalaxy)
	STRTILENAME(purpleLightningID,PurpleLightning)
	STRTILENAME(acidRainID,AcidRain)
	STRTILENAME(whiteNovaID,WhiteNova)
	return nullptr;
}

//图块信息结构体,用于生成地图
struct TileInfo{
	int x,y;
	const char *name,*troop;
	TileInfo():x(0),y(0),name(nullptr),troop(nullptr){}
	void debug(){
		printf("%d,%d",x,y);
		if(name)printf(",%s",name);
		if(troop)printf(",%s",troop);
		printf("\n");
	}
};

static list<TileInfo> tileInfoList;//一张地图的所有地形
static list<TileInfo> unitInfoList;//一张地图的所有单位

static CurlEasy curlEasy;//下载器

string AwbwMap::urlText(int id)const{
	char str[85];
	sprintf(str,"https://awbw.amarriner.com/text_map.php?maps_id=%d",id);
	return str;
}
string AwbwMap::urlHtml(int id)const{
	char str[85];
	sprintf(str,"https://awbw.amarriner.com/prevmaps.php?maps_id=%d",id);
	return str;
}

bool AwbwMap::analyseMapTxt(const string &filename){
	FILE *file=fopen(filename.data(),"r");
	if(!file)return false;
	//准备读取
	char buffer[BUFSIZ];
	int x=0,y=0,val,scaned;
	tileInfoList.clear();
	TileInfo info;
	//开始读取
	while(fgets(buffer,BUFSIZ,file)){//逐行处理
		char *start=strstr(buffer,"<td>");//寻找地形数据
		if(!start)continue;
		start+=4;
		//找到地形数据行
		char *fin=strstr(start,"</td>");
		if(!fin)continue;
		*fin='\0';
		//开始解析行数据
		x=0;
		do{
			scaned=sscanf(start,"%d",&val);//读取
			if(scaned){//有数据
				info.name=strTileName(val);
				info.troop=strTroopName(val);
			}else{//有两个连续的逗号,即逗号间没数据
				info.name=info.troop=nullptr;
			}
			info.x=x;
			info.y=y;
			//info.debug();
			tileInfoList.push_back(info);
			//下一个
			++x;
			start=strstr(start,",");
			if(start){
				++start;
			}
		}while(start);
		++y;
	}
	//保存尺寸
	width=x;height=y;
	fclose(file);
	return true;
}
static void addQuotes(string &str){
	for(SizeType i=0;i<str.size();++i){
		if(str[i]=='\''){
			str.insert(i,"\\");
			i+=2;
		}
	}
}
bool AwbwMap::analyseMapHtml(const string &filename){
	FILE *file=fopen(filename.data(),"r");
	if(!file)return false;
	//准备缓冲,开始分析
	mapName.clear();
	author.clear();
	unitInfoList.clear();
	char buffer[BUFSIZ];
	while(fgets(buffer,BUFSIZ,file)){//逐行处理
		if(mapName.empty()){//地图名先出现
			char *start=strstr(buffer,"maps_id");//过滤出可能有地图名的行
			if(!start)continue;
			//可能有地图名
			start=strstr(start,"\">");
			start+=2;//指向地图名
			char *fin=strstr(start,"</a>");
			if(fin){
				*fin='\0';//把start变成字符串
				while(*start==' ')++start;//去掉所有首空格
				mapName=start;//得到了地图名
			}
		}else if(author.empty()){//作者名在地图名之后
			char *start=strstr(buffer,"username=");
			if(!start)continue;
			//可能有作者名
			start=strstr(start,"\">");
			start+=2;//指向作者名
			char *fin=strstr(start,"</a>");
			if(fin){
				*fin='\0';//把start变成字符串
				author=start;//得到了作者名
			}
		}else{
			char *start,*fin;
			start=strstr(buffer,"<span style=");
			if(!start)continue;
			fin=strstr(start,"</span>");
			if(!fin)continue;
			//得到行数据,开始分析
			analyseMapHtml_line(start);
		}
	}
	fclose(file);
	addQuotes(mapName);
	addQuotes(author);
	return true;
}
void AwbwMap::analyseMapHtml_line(char* line){
	TileInfo info;
	char *start,*fin;

	start=strstr(line,"left:");
	if(!start)return;
	fin=strstr(start,";");
	if(!fin)return;
	start+=5;
	*fin='\0';
	//读取x
	sscanf(start,"%d",&info.x);
	start=fin+1;

	start=strstr(start,"top:");
	if(!start)return;
	fin=strstr(start,";");
	if(!fin)return;
	start+=4;
	*fin='\0';
	//读取y
	sscanf(start,"%d",&info.y);
	start=fin+1;

	//物理坐标变成逻辑坐标(+8是为了取中心以避免误差)
	info.x=(info.x+8)/16;
	info.y=(info.y+8)/16;

	start=strstr(start,"src=");
	if(!start)return;
	fin=strstr(start,".gif");
	if(!fin)return;
	start+=4;
	*fin='\0';
	start=strrchr(start,'/')+1;
	//得到图片主文件名,但要判断是不是单位
	int corpIdx=0,troopIdx=0;
	for(;troopIdx<16;++troopIdx){
		if(memcmp(start,troopNames[troopIdx].shortName,2)==0)break;//搜索部队名
	}
	if(troopIdx>=16)return;//找不到
	for(;corpIdx<25;++corpIdx){
		if(strcmp(start+2,corpNames[corpIdx].shortName)==0)break;//搜索部队名
	}
	if(corpIdx>=25)return;
	//是单位,开始设定单位信息
	info.name=corpNames[corpIdx].name;
	info.troop=troopNames[troopIdx].name;
	//info.debug();
	unitInfoList.push_back(info);
}
void AwbwMap::outputMap(const string &filename){
	//写LUA文件
	FILE *file=fopen(filename.data(),"wb");
	if(!file)return;
	//基本信息
	int w=width;
	int h=height;
	addQuotes(mapName);
	fprintf(file,"%s\n",mapName.data());
	fprintf(file,"%s\n",author.data());
	fprintf(file,"%d,%d\n",w,h);
	//打印地形信息
	for(auto info:tileInfoList){
		if(!info.name)continue;//忽略
		if(info.troop){
			fprintf(file,"%s.%s",info.name,info.troop);
		}else{
			fprintf(file,"%s",info.name);
		}
		//打印逗号或者换行
		if(info.x+1<w){
			fprintf(file,",");
		}else if(info.y+1<h){
			fprintf(file,"\n");
		}
	}
	//打印单位信息
	for(auto info:unitInfoList){
		if(!info.name)continue;//忽略
		fprintf(file,"\n");
		if(info.troop){
			fprintf(file,"%d,%d,%s,%s",info.x,info.y,info.name,info.troop);
		}else{
			fprintf(file,"%d,%d,%s",info.x,info.y,info.name);
		}
	}
	//写入完毕
	fflush(file);
	fclose(file);
}