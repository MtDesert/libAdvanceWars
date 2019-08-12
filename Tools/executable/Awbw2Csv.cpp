#include"AwbwCategory.h"
#include"AwbwMap.h"

void printHelp(){
	printf("语法\n");
	printf("本程序名 file 编号文件 保存路径\n");
}

static AwbwCategory category;

int main(int argc,char* argv[]){
	//检查参数数量
	if(argc<2){
		printHelp();
		return -1;
	}
	//确定模式
	string mode=argv[1];
	if(mode=="file" && argc==4){
		auto filename=argv[2];
		auto savePath=argv[3];
		//生成id表
		category.loadFile(filename);
		category.convertAllMaps(savePath);
	}else{
		printHelp();
		return -1;
	}
	return 0;
}