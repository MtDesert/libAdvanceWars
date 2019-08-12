#ifndef DATALIST_H
#define DATALIST_H

#include"List.h"
#include<string>
using namespace std;

//List的特别版,针对特定数据类型T,T类型中应该带有std::string name成员
template<typename T>
class DataList:public List<T>{
protected:
	ListNode<T>* nodeName(const string &name,uint &pos)const{
		auto n=this->head;
		uint p=0;
		for(;n;n=n->next,++p){
			if(n->data.name==name)break;
		};
		pos=p;
		return n;
	}
public:
	const T* dataName(const string &name,uint &pos)const{
		auto n=nodeName(name,pos);
		return n?(&n->data):nullptr;
	}
	T* dataName(const string &name,uint &pos){
		auto n=nodeName(name,pos);
		return n?(&n->data):nullptr;
	}
};

#endif
