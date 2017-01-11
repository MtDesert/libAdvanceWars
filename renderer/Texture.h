#ifndef TEXTURE_H
#define TEXTURE_H

#include<gl.h>
#include"FileBMP.h"
#include"FilePNG.h"

class Texture
{
public:
	Texture();
	~Texture();

	void setTexImage2D(GLsizei width,GLsizei height,const GLvoid* pixels);

	void setTexImage2D(const FileBMP &fileBmp);
	void setTexImage2D(const FilePNG &filePng);
	void setTexImage2D(const Bitmap_32bit &bitmap);
	void draw();
private:
	GLuint texture;
};

#endif // TEXTURE_H