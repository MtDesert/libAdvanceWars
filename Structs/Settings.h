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
	//数据文件名以及图片路径
#define DATA_FILENAME_AND_IMAGEPATH(name)\
	EnumType filenames##name;\
	string imagePath##name;

	DATA_FILENAME_AND_IMAGEPATH(Corps)
	DATA_FILENAME_AND_IMAGEPATH(Troops)
	DATA_FILENAME_AND_IMAGEPATH(Commanders)
	string imagePathCommandersBodies;
	DATA_FILENAME_AND_IMAGEPATH(TerrainCode)
#undef DATA_FILENAME_AND_IMAGEPATH

	//设定数据
	bool battleAnimation;
	bool mapAnimation;
	bool showGrid;

	//保存/加载设置
	bool saveFile(const string &filename)const;
	bool loadFile(const string &filename);
};

#endif