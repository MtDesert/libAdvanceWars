#ifndef TEXTURES_H
#define TEXTURES_H

#include"gl.h"
#include<vector>
using namespace std;

struct Textures{
	//generate and delete
	void genTextures(GLint amount);
	void deleteTextures();
	
	bool activeTexture(size_t index)const;
	bool bindTexture(size_t index)const;
	bool texture(size_t index,GLuint &value)const;
private:
	vector<GLuint> textures;
};

#endif