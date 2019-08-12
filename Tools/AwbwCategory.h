#ifndef AWBWCATEGORY_H
#define AWBWCATEGORY_H

#include"Set.h"
#include<string>
using namespace std;

//分类的名字和对应的id号
struct AwbwCategory{
	int id;//分类的ID,生成网址用
	uint lastestAmount;//最新的数量
	string name;//字符串形成的名字,管理文件夹用
	Set<int> allMapsID;//所有的地图的编号
	//静态变量
	static string categoriesPath;//分组数据保存路径
	static string mapsPath;//地图数据保存路径

	AwbwCategory();
	//字符串
	static uint pageStart(uint idAmount);//根据id数推断开始更新的页数
	string urlCategoryPage(int page=0)const;//生成网址(当前页)
	static string urlText(int mapID);//生成网址(地形文本)
	static string urlHtml(int mapID);//生成网址(单位文本)
	string pageFilename(int page=0)const;//页面文件名,分析用
	string dataFilename()const;//数据文件名,保存分析结果用
	string mapFilename(int mapID,const string &suffix)const;//地图文件名(id,后缀)

	//页面文件操作(html文件名)
	bool downloadPage(int page=0)const;//下载页面文件,自动跳过已经下载的文件,返回是否成功
	int parsePageFile(int page=0);//分析页面文件,分析结果添加到allMapsID中
	bool removePageFile(int page=0)const;//删除页面文件
	void clearPageCache();//清除更新缓冲

	//数据文件操作(txt文件名),读取/保存数据,返回是否成功
	bool saveFile(const string &filename)const;
	bool loadFile(const string &filename);

	//地图文件操作
	void downloadMap(int mapID);//下载地图
	void parseMap(int mapID);//转码地图
	void removeMapTxtHtml(int mapID);//删除地图对应的txt和html文件
	bool existMapFile(int mapID)const;//返回是否存在地图id文件

	//更新(txt数据文件)
	void updateMapsID();//更新地图ID
	void updateMapsID(int fromPage,int toPage);//从fromPage到toPage更新地图ID
	int updatableAmount()const;//可更新数
	int incrementalUpdate();//增量更新
	int fullVolumeUpdate();//全量更新

	//根据allMapsID转换所有地图(保存在mapsPath中)
	void convertAllMaps(const string &mapsPath)const;
};

//所有分组
struct AwbwAllCategories{
	List<AwbwCategory> allCategories;//所有分组的数据

	AwbwAllCategories();
	//搜索分组
	AwbwCategory* findCategoryByID(int id)const;
	//分组信息操作
	bool loadFile(const string &filename);//加载分组文件,以确定要操作的分组
	void loadAllCategoriesFiles();//加载需要操作的分组,获取各个分组的id列表
	void updateAllCategories();//更新各个分组数据
	void clearAllCache();//清除所有更新缓冲
	//去重,保证每个地图id只出现在一个分组中
	void uniqueMapsIDforAllCategories();
	//地图操作
	string existMapFile(int mapID)const;//是否存在mapID地图,若存在则返回所在分组名,不存在则返回空字符串
	void updateAllMaps();//更新所有需要更新的地图
};
#endif