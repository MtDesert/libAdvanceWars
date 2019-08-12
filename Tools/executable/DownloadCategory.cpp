#include"AwbwCategory.h"

void printHelp(){
	printf("语法: 本程序名 分类文件路径 分类文件名\n");
	printf("使用前,请先去awbw查看各个分类名的路径,以获取url中的id\n");
}

int main(int argc,char* argv[]){
	//检查参数数量
	if(argc==3){
		//获取参数
		auto categoriesPath=argv[1];
		auto filename=argv[2];
		//打开文件,加载所有分组
		AwbwAllCategories allCategories;
		AwbwCategory::categoriesPath=categoriesPath;
		if(!allCategories.loadFile(filename)){//加载文件
			perror("加载分组文件失败");
			return -1;
		}
		allCategories.loadAllCategoriesFiles();//加载所有分组的ID
		allCategories.updateAllCategories();//更新所有分组数据
		allCategories.clearAllCache();//清除缓冲
	}else{
		printHelp();
		return -1;
	}
	//等待完成
	return 0;
}
