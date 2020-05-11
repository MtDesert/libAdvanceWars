#include"AwbwCategory.h"
#include"AwbwMap.h"
#include"CurlEasy.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
//没有itoa(),自己实现一个
string int2Str(int i){
	char str[12];
	sprintf(str,"%d",i);
	return str;
}

static CurlEasy curlEasy;//下载器
static bool assending(const int &a,const int &b){return a<b;}//排序用的函数

int systemRet=0;//专门接收system()返回值
#define system(arg) systemRet=::system(arg);//应对-Werror编译选项而改写

#define MAPS_COUNT_PER_PAGE 25

AwbwCategory::AwbwCategory():id(0),lastestAmount(0){}
AwbwAllCategories::AwbwAllCategories(){}
string AwbwCategory::categoriesPath(".");
string AwbwCategory::mapsPath(".");

uint AwbwCategory::pageStart(uint idAmount){return idAmount/MAPS_COUNT_PER_PAGE;}
string AwbwCategory::urlCategoryPage(int page)const{
	char str[128];
	sprintf(str,"https://awbw.amarriner.com/categories.php?categories_id=%d&start=%d",id,MAPS_COUNT_PER_PAGE*page+1);
	return str;
}
string AwbwCategory::urlText(int id){
	char str[85];
	sprintf(str,"https://awbw.amarriner.com/text_map.php?maps_id=%d",id);
	return str;
}
string AwbwCategory::urlHtml(int id){
	char str[85];
	sprintf(str,"https://awbw.amarriner.com/prevmaps.php?maps_id=%d",id);
	return str;
}

string AwbwCategory::pageFilename(int page)const{
	return categoriesPath+"/"+name.data()+int2Str(page)+".html";
}
string AwbwCategory::dataFilename()const{return categoriesPath+"/"+name.data()+".txt";}
string AwbwCategory::mapFilename(int mapID, const string &suffix)const{
	return mapsPath+"/"+name+"/"+int2Str(mapID)+suffix;
}

bool AwbwCategory::downloadPage(int page)const{
	auto htmlFilename=pageFilename(page);
	if(access(htmlFilename.data(),0)==0){
		printf("found %s\n",htmlFilename.data());
		return true;
	}else{
		auto strUrl=urlCategoryPage(page);//设定链接
		bool downloadOK=false;
		while(!downloadOK){//下载直到成功...
			downloadOK=(curlEasy.downloadFile(strUrl,htmlFilename)==CURLE_OK);//下载索引文件
		}
		return downloadOK;
	}
}

int AwbwCategory::parsePageFile(int page){
	FILE *file=fopen(pageFilename(page).data(),"r");
	if(!file)return 0;
	//准备缓冲,开始分析
	int count=0;//解析出的地图数
	char buffer[BUFSIZ];
	lastestAmount=0;//在网页中继续解析
	while(fgets(buffer,BUFSIZ,file)){//逐行处理
		if(lastestAmount==0){//扫描最新的地图数
			string str=CurlEasy::parseContent(buffer,"yellow_text_plain>(","&nbsp",false);
			if(!str.empty()){
				lastestAmount=atoi(str.data());
			}
		}else{//扫描地图id
			string str=CurlEasy::parseContent(buffer,"prevmaps.php?maps_id=",">",false);
			if(!str.empty()){
				allMapsID.insert(atoi(str.data()));
				++count;
			}
		}
	}
	fclose(file);
	return count;
}
bool AwbwCategory::removePageFile(int page)const{
	return remove(pageFilename(page).data())==0;
}
void AwbwCategory::clearPageCache(){
	auto amount=pageStart(lastestAmount);
	for(uint i=0;i<=amount;++i){
		removePageFile(i);
	}
}

//保存数据(文本文件名),返回是否成功
bool AwbwCategory::saveFile(const string &filename)const{
	FILE *file=fopen(filename.data(),"w");
	if(!file)return false;
	//开始写入
	for(auto num:allMapsID){
		fprintf(file,"%d\n",num);
	}
	//写入完毕
	fflush(file);
	fclose(file);
	return true;
}
//读取数据(文本文件名),返回是否成功
bool AwbwCategory::loadFile(const string &filename){
	FILE *file=fopen(filename.data(),"r");
	if(!file)return false;
	//开始读取
	allMapsID.clear();
	int num;
	while(fscanf(file,"%d",&num)!=EOF){
		allMapsID.insert(num);
	}
	fclose(file);
	return true;
}

void AwbwCategory::downloadMap(int mapID){
	auto txtFilename=mapFilename(mapID,".txt");
	if(curlEasy.downloadFile(urlText(mapID),txtFilename)==CURLE_OK){
		AwbwMap awbwMap;
		awbwMap.analyseMapTxt(txtFilename);
		if(awbwMap.width && awbwMap.height){//有效地图,下载之
			curlEasy.downloadFile(urlHtml(mapID),mapFilename(mapID,".html"));
		}else{//无效地图,删除之
			remove(txtFilename.data());
		}
	}
}
void AwbwCategory::parseMap(int mapID){
	auto filename=mapFilename(mapID,"");
	AwbwMap awbwMap;
	if(awbwMap.analyseMapTxt(filename+".txt")){
		if(awbwMap.analyseMapHtml(filename+".html")){
			//解析成功后才输出
			awbwMap.outputMap(filename+".csv");
		}
	}
}
void AwbwCategory::removeMapTxtHtml(int mapID){
	auto filename=mapFilename(mapID,"");
	remove((filename+".txt").data());
	remove((filename+".html").data());
}
bool AwbwCategory::existMapFile(int mapID)const{return access(mapFilename(mapID,".csv").data(),0)==0;}

int AwbwCategory::updatableAmount()const{
	return lastestAmount-allMapsID.size();
}
int AwbwCategory::incrementalUpdate(){
	printf("增量更新\n");
	int page=pageStart(allMapsID.size());//需要更新的起始页
	int targetPage=pageStart(lastestAmount);
	updateMapsID(page,targetPage);
	return 0;
}
int AwbwCategory::fullVolumeUpdate(){
	printf("全量更新\n");
	int targetPage=pageStart(lastestAmount);
	updateMapsID(0,targetPage);
	return 0;
}

void AwbwCategory::updateMapsID(){
	loadFile(dataFilename());//重新读取
	updateMapsID(0,0);//先拿首页,目的是为了拿分组的地图数
	//判断是否需要更新
	auto amount=updatableAmount();
	if(amount>0){
		//先执行增量更新
		incrementalUpdate();
		if(updatableAmount()){//增量还不够,那么只好全量更新了
			fullVolumeUpdate();
		}
		//更新完成后,排序保存
		allMapsID.sort(assending);
		saveFile(dataFilename());
	}else if(amount<0){
		//这里应该执行清理
		printf("need clean: %s(id=%d,amount=%d)\n",name.data(),id,lastestAmount);
	}
}
void AwbwCategory::updateMapsID(int fromPage,int toPage){
	for(int i=fromPage;i<=toPage;++i){
		if(downloadPage(i)){//下载成功后
			parsePageFile(i);//分析数据
			saveFile(dataFilename());//立刻保存结果
			//如果增量更新已经结束,则中断
			if(updatableAmount()<=0)break;
		}
	}
}

void AwbwCategory::convertAllMaps(const string &mapsPath)const{
	char strID[10];
	string filename,csvFilename;
	for(auto id:allMapsID){
		sprintf(strID,"%d",id);
		//生成文件名
		filename=mapsPath+"/"+strID;
		csvFilename=filename+".csv";
		if(access(csvFilename.data(),0)==0){
			printf("存在文件%s\n",csvFilename.data());
		}else{
			AwbwMap awbwMap;
			awbwMap.analyseMapTxt(filename+".txt");
			awbwMap.analyseMapHtml(filename+".html");
			awbwMap.outputMap(csvFilename);
		}
	}
}

#define FOR_ALL_CATEGORIES(code) \
for(auto &category:allCategories){\
	code;\
}

AwbwCategory* AwbwAllCategories::findCategoryByID(int id)const{
	FOR_ALL_CATEGORIES(if(category.id==id)return &category)
	return nullptr;
}

#define BUFFER_SIZE 256
bool AwbwAllCategories::loadFile(const string &filename){
	FILE *file=fopen((AwbwCategory::categoriesPath+"/"+filename).data(),"r");
	if(!file)return false;
	//读取分组
	char buffer[BUFFER_SIZE];
	while(fgets(buffer,BUFFER_SIZE,file)){//逐行读取
		if(memcmp(buffer,"/",1)==0){//注释,跳过
		}else{
			auto comma=strstr(buffer,",");
			if(comma){
				*comma='\0';//转字符串
				++comma;//指向下一个数字
				//读取数据
				AwbwCategory category;
				category.id=atoi(comma);
				category.name=buffer;
				allCategories.push_back(category);
			}
		}
	}
	//读取完毕
	fclose(file);
	return true;
}

void AwbwAllCategories::loadAllCategoriesFiles(){FOR_ALL_CATEGORIES(category.loadFile(category.dataFilename()))}
void AwbwAllCategories::updateAllCategories(){FOR_ALL_CATEGORIES(category.updateMapsID())}
void AwbwAllCategories::clearAllCache(){FOR_ALL_CATEGORIES(category.clearPageCache())}

void AwbwAllCategories::uniqueMapsIDforAllCategories(){
	for(auto itrA=allCategories.begin();itrA!=allCategories.end();++itrA){
		for(auto itrB=allCategories.begin();itrB!=itrA;++itrB){
			//开始去重
			for(auto itr=itrB->allMapsID.begin();itr!=itrB->allMapsID.end();++itr){
				itrA->allMapsID.remove(*itr);
			}
		}
	}
}
string AwbwAllCategories::existMapFile(int mapID)const{
	string ret;
	FOR_ALL_CATEGORIES(
		if(category.existMapFile(mapID)){
			ret=category.name;
			break;
		}
	)
	return ret;
}
void AwbwAllCategories::updateAllMaps(){
	//处理每个分组的每张地图
	string existPath;
	for(auto &category:allCategories){
		system((string("mkdir ")+AwbwCategory::mapsPath+"/"+category.name).data());
		printf("%s\n",category.name.data());
		for(auto &id:category.allMapsID){
			//根据存在的文件夹来执行对应的动作
			existPath=existMapFile(id);
			if(existPath.empty()){//不存在,需要下载
				category.downloadMap(id);
				category.parseMap(id);
			}else{//存在,但要看看是否需要变更位置
				if(existPath!=category.name){//需要移动文件
					printf("需要移动文件%d\n",id);
				}
			}
		}
	}
}