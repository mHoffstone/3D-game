#ifndef RENDERABLETEXTURE_H
#define RENDERABLETEXTURE_H

#include "texture.h"

class RenderableTexture : public Texture{
	public:
		RenderableTexture();
		virtual ~RenderableTexture();

		RenderableTexture& operator=(const Texture& rhs);

		void create(int filter, int wrap);
		void bindTexture(unsigned int slot = 0) const;
		void apply() const;

		unsigned int id = 0;

	protected:

	private:
};

#endif // RENDERABLETEXTURE_H
