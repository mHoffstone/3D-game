#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Texture{
	public:
		Texture();
		Texture(const Texture& from);
		virtual ~Texture();

		void readImage(const std::string& path);

		int width, height;
		unsigned char* data = nullptr;

	protected:

	private:
};

#endif // TEXTURE_H
