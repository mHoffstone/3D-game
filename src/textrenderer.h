#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include "engine/texture.h"
#include "engine/vec.h"
#include "engine/renderer.h"
#include "engine/renderabletexture.h"

#include "engine/objectlife.h"

#include <string>

class TextRenderer : public ObjectLife{
public:
	TextRenderer();
	virtual ~TextRenderer();

	virtual void create(const Texture& fontSheet);
	virtual void render(Renderer& r, const std::string& text, vec2f pos);

	virtual void destroy() override;

protected:
	unsigned int vao, vbo;
	Program program;
	int programId;
	RenderableTexture fontTexture;
	Uniform translation;
	Uniform vpSize;
	Uniform textUniform;
	Uniform sampler;
	bool hasCreated = false;
};

#endif // TEXTRENDERER_H
