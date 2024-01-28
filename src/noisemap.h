#ifndef NOISEMAP_H
#define NOISEMAP_H

#include <memory>

class NoiseMap{
private:
	NoiseMap();
	virtual ~NoiseMap();

public:
	static float valueAt(float x, float y, int seed);
	static std::unique_ptr<float[]> generateMap(int x, int y, int width, int height, float octave, int seed);
	static std::unique_ptr<float[]> power(std::unique_ptr<float[]>, int width, int height, float exponent);
};

#endif // NOISEMAP_H
