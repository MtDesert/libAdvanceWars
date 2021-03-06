#ifndef SETTINGS_H
#define SETTINGS_H

#include"GameSettings.h"

//高战的游戏设置数据,在游戏启动时应该先读取本数据
class Settings:public GameSettings{
public:
	//构造/析构函数
	Settings();
	~Settings();

	string mapsPath;//地图路径
	string scenarioScriptsPath;//剧情脚本文件的路径
	string savesPath;//存档路径
	//数据文件名以及图片路径
#define DATA_FILENAME_AND_IMAGEPATH(name)\
	string data##name;\
	string imagesPath##name;

	DATA_FILENAME_AND_IMAGEPATH(Corps)
	DATA_FILENAME_AND_IMAGEPATH(Troops)
	DATA_FILENAME_AND_IMAGEPATH(Commanders)
	DATA_FILENAME_AND_IMAGEPATH(TerrainCodes)
	DATA_FILENAME_AND_IMAGEPATH(Weathers)
	string imagesPathCorpsImages;
#undef DATA_FILENAME_AND_IMAGEPATH
	string ruleMove;//移动规则文件
	string ruleDamage;//损伤规则文件
	string ruleLoadUnit;//装载单位规则
	string ruleBuild;//建造规则
	string ruleCommanders;//指挥官规则

	string imagesPathIcons;//兵种命令菜单图表路径
	string imagesPathNumbers;//数字图片路径

	//设定数据
	bool battleAnimation;//战斗动画是否显示
	bool mapAnimation;//地图动画是否播放
	bool showGrid;//是否显示网格线

	//保存/加载设置
	bool saveFile(const string &filename)const;
protected:
	void readCustom(LuaState &state);
};
#endif