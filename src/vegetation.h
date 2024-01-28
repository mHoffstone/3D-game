#ifndef VEGETATION_H
#define VEGETATION_H

#include "engine/mesh.h"

#include <vector>

struct tree_parameter{
	float width; // Typ. width
	float height; // Typ. height
	float dev; // Max deviation scale factor
	float scale; // Scale factor
	float a, b, c, d, e;
};

class Vegetation{
public:
	Vegetation();

	int addTreeType(tree_parameter tp);
	int addTreeType();

	float getHeight(int type);

	Mesh generateTree(int type);



	static Vegetation& instance();

private:
	std::vector<tree_parameter> treeParameters;
};

#endif // VEGETATION_H
