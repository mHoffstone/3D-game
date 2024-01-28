#ifndef RENDERABLETEXTUREARRAY_H
#define RENDERABLETEXTUREARRAY_H

#include "texture.h"

#include <vector>

class RenderableTextureArray{
public:
	RenderableTextureArray();
	virtual ~RenderableTextureArray();
	void create(int width, int height, int filter, int wrap);
	void addTexture(const Texture& texture);
	void apply() const;
	void bindTexture(unsigned int slot = 0) const;
	const Texture& at(int i) const;

	int getWidth() const;
	int getHeight() const;

private:
	std::vector<Texture> textures;
	int width, height;
	unsigned int id = 0;
};

#endif // RENDERABLETEXTUREARRAY_H
