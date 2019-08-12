#include"AwbwMap.h"
#include"AwbwCategory.h"

#include<list>
using namespace std;
#include<stdlib.h>
#include<unistd.h>

void printHelp(){
	printf("语法\n");
	printf("本程序名 id 起始地图id 结束地图id 过滤路径 保存路径\n");
	printf("本程序名 分类文件路径 分类文件名 地图保存路径\n");
}

//升序排序用
int main(int argc,char* argv[]){
	//检查参数数量
	if(argc<2){
		printHelp();
		return -1;
	}
	//确定模式
	string mode=argv[1];
	if(mode=="id" && argc==6){
		auto fromID=atoi(argv[2]);
		auto toID=atoi(argv[3]);
		//auto existPath=argv[4];
		//auto savePath=argv[5];
		//生成id表
		Set<int> allMapsID;
		for(int id=fromID;id<=toID;++id){
			allMapsID.insert(id);
		}
	}else if(argc==4){
		//参数表
		auto categoriesPath=argv[1];
		auto filename=argv[2];
		auto mapsPath=argv[3];
		//设置路径
		AwbwCategory::categoriesPath=categoriesPath;
		AwbwCategory::mapsPath=mapsPath;
		//加载相关文件
		AwbwAllCategories allCategories;
		allCategories.loadFile(filename);
		allCategories.loadAllCategoriesFiles();
		allCategories.uniqueMapsIDforAllCategories();//这里做ID去重,因为一个ID可能会属于多个分组
		allCategories.updateAllMaps();//开始更新所有地图
	}else{
		printHelp();
		return -1;
	}
	//等待完成
	return 0;
}