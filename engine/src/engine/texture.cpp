#include "texture.h"

#include "util.h"

Texture::Texture(){
	//ctor
}
Texture::Texture(const Texture& from){
	width = from.width;
	height = from.height;
	if(from.data == nullptr) data = nullptr;
	else{
		data = new unsigned char[width*height*4];
		for(int i = 0; i < width*height*4; i++) data[i] = from.data[i];
	}
}


Texture::~Texture(){
	if(data != nullptr) delete[] data;
}

void Texture::readImage(const std::string& path){
	std::string fdata = util::readFile(path);

	auto toInt = [](unsigned char a, unsigned char b){
		return (int)a + (((int)b) << 8);
	};

	int imgType = int(fdata[2]);
	width = toInt(fdata[12], fdata[13]);
	height = toInt(fdata[14], fdata[15]);
	int depth = int(fdata[16]);

	if(imgType != 2){
		std::cout << "Image type not supported!" << std::endl;
		width = -1;
		height = -1;
		return;
	}

	int i = 18;
	data = new unsigned char[width*height*4];
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			data[4*(y*width + x)+2] = fdata[i++];
			data[4*(y*width + x)+1] = fdata[i++];
			data[4*(y*width + x)+0] = fdata[i++];
			if(depth == 32){
				data[4*(y*width + x)+3] = fdata[i++];
			}
			else{
				data[4*(y*width + x)+3] = 255;
			}
		}
	}
}
