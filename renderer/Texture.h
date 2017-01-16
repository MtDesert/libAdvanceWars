#ifndef TEXTURE_H
#define TEXTURE_H

#include<gl.h>
#include"FileBMP.h"
#include"FilePNG.h"
#include"Point.h"

class Texture
{
public:
	Texture();
	~Texture();

	void texImage2D(GLsizei width,GLsizei height,const GLvoid* pixels);
	void deleteTexture();

	void texImage2D(const FileBMP &fileBmp);
	void texImage2D(const FilePNG &filePng);
	void texImage2D(const Bitmap_32bit &bitmap);

	void draw(const Point2D<GLfloat> &p)const;
	void draw(const Point2D<GLfloat> &p,const Point2D<GLfloat> &size)const;
private:
	GLuint texture;
	GLsizei width,height;
};

#endif // TEXTURE_H