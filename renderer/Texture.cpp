#include "Texture.h"

Texture::Texture():texture(0),width(0),height(0){}
Texture::~Texture(){}

void Texture::texImage2D(GLsizei width, GLsizei height, const GLvoid *pixels)
{
	if(!glIsTexture(texture)){
		glGenTextures(1,&texture);
	}
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	this->width=1;//width;
	this->height=1;//height;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}
void Texture::deleteTexture(){
	if(glIsTexture(texture))glDeleteTextures(1,&texture);
}
void Texture::texImage2D(const FileBMP &fileBmp){
	Bitmap_32bit bitmap;
	fileBmp.decodeTo(bitmap);
	texImage2D(bitmap);
}
void Texture::texImage2D(const FilePNG &filePng){
	Bitmap_32bit bitmap;
	filePng.decodeTo(bitmap);
	texImage2D(bitmap);
}
void Texture::texImage2D(const Bitmap_32bit &bitmap){
	texImage2D(bitmap.getWidth(),bitmap.getHeight(),bitmap.dataPointer);
}
void Texture::draw(const Point2D<GLfloat> &p)const{
	draw(p,Point2D<GLfloat>(width,height));
}
void Texture::draw(const Point2D<GLfloat> &p,const Point2D<GLfloat> &size)const{
	glBindTexture(GL_TEXTURE_2D,texture);
	//vertex
	GLfloat vertex[]={p.x,p.y , p.x+size.x,p.y , p.x+size.x,p.y+size.y , p.x,p.y+size.y};
	glVertexPointer(2,GL_FLOAT,0,vertex);
	//texCoord
	GLfloat texCoord[]={0,0 , 1,0 , 1,1 , 0,1};
	glTexCoordPointer(2,GL_FLOAT,0,texCoord);
	//draw
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
}