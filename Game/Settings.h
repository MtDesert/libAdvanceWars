#ifndef SETTINGS
#define SETTINGS

#include"EnumType.h"
#include"StringList.h"
#include"Map.h"

//高战的游戏设置数据,在游戏启动时应该先读取本数据
class Settings{
public:
	//构造/析构函数
	Settings();
	~Settings();

	string language;//语言名称
	string mapsPath;//地图路径
	string senarioScriptsPath;//剧情脚本文件的路径
	//数据文件名以及图片路径
#define DATA_FILENAME_AND_IMAGEPATH(name)\
	string Filename##name;\
	string ImagesPath##name;

	DATA_FILENAME_AND_IMAGEPATH(Corps)
	DATA_FILENAME_AND_IMAGEPATH(Troops)
	DATA_FILENAME_AND_IMAGEPATH(Commanders)
	DATA_FILENAME_AND_IMAGEPATH(Terrains)
#undef DATA_FILENAME_AND_IMAGEPATH

	//设定数据
	bool battleAnimation;//战斗动画是否显示
	bool mapAnimation;//地图动画是否播放
	bool showGrid;//是否显示网格线
	//网络部分
	string serverAddress;
	int serverPort;

	//保存/加载设置
	bool saveFile(const string &filename)const;
	bool loadFile(const string &filename,void (*whenError)(const string &errStr));
};

#endif