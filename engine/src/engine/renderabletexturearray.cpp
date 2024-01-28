#include "renderabletexturearray.h"

#include <GL/glew.h>
#include <stdexcept>
#include "util.h"

RenderableTextureArray::RenderableTextureArray(){

}

RenderableTextureArray::~RenderableTextureArray(){
	if(id != 0) glDeleteTextures(1, &id);
}
void RenderableTextureArray::create(int width, int height, int filter, int wrap){
	this->width = width;
	this->height = height;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrap);
}
void RenderableTextureArray::addTexture(const Texture& texture){
	if(texture.width == width && texture.height == height){
		textures.push_back(texture);
	}
	else throw std::runtime_error("Texture size did not match");
}
void RenderableTextureArray::apply() const{
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, textures.size());
	for(int i = 0; i < (int)textures.size(); i++){
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, textures[i].data);
	}

}
void RenderableTextureArray::bindTexture(unsigned int slot) const{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}
const Texture& RenderableTextureArray::at(int i) const{
	return textures[i];
}
int RenderableTextureArray::getWidth() const{
	return width;
}
int RenderableTextureArray::getHeight() const{
	return height;
}



