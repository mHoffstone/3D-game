#include "renderabletexture.h"

#include <GL/glew.h>

RenderableTexture::RenderableTexture(){

}

RenderableTexture::~RenderableTexture(){
	if(id != 0) glDeleteTextures(1, &id);
}

RenderableTexture& RenderableTexture::operator=(const Texture& rhs){
	width = rhs.width;
	height = rhs.height;
	data = new unsigned char[width*height*4];
	for(int i = 0; i < width*height*4; i++) data[i] = rhs.data[i];
	return *this;
}

void RenderableTexture::create(int filter, int wrap){
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
}
void RenderableTexture::bindTexture(unsigned int slot) const{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);
}

void RenderableTexture::apply() const{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}
