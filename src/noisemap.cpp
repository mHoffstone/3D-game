#include "noisemap.h"

#include "engine/util.h"

#include <cmath>

static unsigned int pcg_hash(unsigned int input){
	unsigned int state = input * 747796405u + 2891336453u;
	unsigned int word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}


static float rand2(int layer, int x, int y){
	unsigned int s = pcg_hash(layer);
	s = pcg_hash(s + x);
	s = pcg_hash(s + y);
	return float(s % 10000)/10000.0f;

	/*std::seed_seq seed{layer, x, y};
	 * std::mt19937 generator(seed);
	 * std::uniform_real_distribution<float> random(0.0, 1.0);
	 * return random(generator);*/

	//srand(x*10000+y*100+layer);
	//return rand() % 1000 / 1000.0f;
	//return 0.0f;
}

NoiseMap::NoiseMap()
{
}

NoiseMap::~NoiseMap()
{
}

float NoiseMap::valueAt(float x, float y, int seed){
	float xl = std::floor(x), yl = std::floor(y);
	float v00=rand2(seed, xl,yl), v10=rand2(seed, xl+1,yl), v11=rand2(seed, xl+1,yl+1), v01=rand2(seed, xl,yl+1);
	return util::berp(v00, v01, v10, v11, (float)(x-xl), (float)(y-yl), util::cerp);
	float bx = x-xl, by = y-yl;
	float vx0 = util::cerp(v00, v10, bx), vx1 = util::cerp(v01, v11, bx);
	return util::cerp(vx0, vx1, by);
}

std::unique_ptr<float[]> NoiseMap::generateMap(int x, int y, int width, int height, float octave, int seed){
	std::unique_ptr<float[]> data(new float[width*height]);
	for(int j = 0; j < height; j++){
		for(int i = 0; i < width; i++){
			data[j*width+i] = valueAt((x+i)/octave, (y+j)/octave, seed);
		}
	}
	return data;
}

std::unique_ptr<float[]> NoiseMap::power(std::unique_ptr<float[]> data, int width, int height, float exponent){
	for(int j = 0; j < height; j++){
		for(int i = 0; i < width; i++){
			data[j*width+i] = std::pow(data[j*width+i], exponent);
		}
	}
	return data;
}

