#include "Texture.h"

Texture::Texture()
{
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
}
Texture::~Texture()
{
	glDeleteTextures(1,&texture);
}

void Texture::setTexImage2D(GLsizei width, GLsizei height, const GLvoid *pixels)
{
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}
void Texture::setTexImage2D(const FileBMP &fileBmp){
	//make texture
	Bitmap_32bit bitmap;
	fileBmp.decodeTo(bitmap);
	setTexImage2D(bitmap);
}
void Texture::setTexImage2D(const Bitmap_32bit &bitmap){
	setTexImage2D(bitmap.getWidth(),bitmap.getHeight(),bitmap.dataPointer);
}
void Texture::draw(){
	glBindTexture(GL_TEXTURE_2D,texture);
	//vertex
	GLfloat vertex[]={0,0 , 1,0 , 1,1 , 0,1};
	glVertexPointer(2,GL_FLOAT,0,vertex);
	//texCoord
	GLfloat texCoord[]={0,0 , 1,0 , 1,1 , 0,1};
	glTexCoordPointer(2,GL_FLOAT,0,texCoord);
	//draw
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
}
