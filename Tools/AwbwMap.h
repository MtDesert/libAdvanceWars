#ifndef AWBWMAP_H
#define AWBWMAP_H

#include<string>
using namespace std;

//awbw地图,用于下载和分析数据
struct AwbwMap{
	int width,height;//地图长宽
	string mapName,author;//地图名,作者

	void analyseMapHtml_line(char *line);//分析地图行
	//生成网址
	string urlText(int id=0)const;
	string urlHtml(int id=0)const;
	//分析地图,保存分析结果于内存中
	bool analyseMapTxt(const string &filename);//实质是带csv内容的网页格式
	bool analyseMapHtml(const string &filename);
	//从内存中将结果输出成文件
	void outputMap(const string &filename);
};
#endif