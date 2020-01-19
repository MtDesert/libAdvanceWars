#ifndef DATALIST_H
#define DATALIST_H

#include"Array.h"

//数据表(主要保存配置数据),针对特定数据类型T,T类型中应该带有std::string name成员
template<typename T>
class DataList:public Array<T>{
public:
	const T* dataName(const string &name,SizeType &pos)const{
		auto n=this->indexOf([name](const T &val){return val.name==name;});
		if(n>=0){
			pos=n;
			return this->data(n);
		}
		return nullptr;
	}
};
#endif