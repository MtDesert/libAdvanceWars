#include"Textures.h"

void Textures::genTextures(GLint amount){
	deleteTextures();
	textures.resize(amount);
	glGenTextures(amount,textures.data());
}
void Textures::deleteTextures(){
	glDeleteTextures(textures.size(),textures.data());
	textures.clear();
}

bool Textures::activeTexture(size_t index)const{
	if(index>=textures.size())return false;
	glActiveTexture(textures[index]);
	return true;
}
bool Textures::bindTexture(size_t index)const{
	if(index>=textures.size())return false;
	glBindTexture(GL_TEXTURE_2D,textures[index]);
	return true;
}
bool Textures::texture(size_t index, GLuint &value)const{
	if(index>=textures.size())return false;
	value=textures[index];
	return true;
}