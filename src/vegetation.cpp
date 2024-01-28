#include "vegetation.h"

#include <cmath>

static Vegetation vegetationInstance;

Vegetation::Vegetation(){}


int Vegetation::addTreeType(tree_parameter tp){
	treeParameters.push_back(tp);
	return treeParameters.size()-1;
}

int Vegetation::addTreeType(){
	tree_parameter tp;
	float max = 0.0f;
	while(max <= 0.0f){
		tp.height = rand() % 30 + 5;
		tp.width = rand() % 5 + 3 + tp.height / 10.0f;
		tp.dev = rand() % 20 / 100.0f + 0.2f;
		tp.a = (rand() % 100 - 50) / 5.0f;
		tp.b = (rand() % 100 - 50) / 5.0f;
		tp.c = (rand() % 100 - 50) / 5.0f;
		tp.d = (rand() % 100 - 50) / 5.0f;
		tp.e = rand() % 2;

		int maxi = 0;
		for(int i = 0; i < 100; i++){
			float jf = i / 100.0f;
			float v = tp.a * jf*jf*jf + tp.b * jf*jf + tp.c * jf + tp.d * std::sqrt(-jf*jf + jf);
			if(tp.e == 0.0f) v = std::abs(v);
			else if(v < 0.0f) v = 0.0f;
			if(v > max){
				max = v;
				maxi = i;
			}
		}
	}

	tp.scale = 1.0f / max;

	return addTreeType(tp);
}

float Vegetation::getHeight(int type){
	return treeParameters[type].height;
}


Mesh Vegetation::generateTree(int type){
	tree_parameter tp = treeParameters[type];
	Mesh tree;
	Mesh trunk = Mesh::createCone({0.0f, 0.0f, 0.0f}, 0.5f, tp.height, MeshMode::SUBDIVIDE);
	trunk.setTexture(3);
	tree.add(trunk);
	Mesh plane = Mesh::createXYPlane({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f});
	plane.setTexture(2);
	float levelSpacing = 1.0f;
	int nLevels = tp.height / levelSpacing;
	for(int j = 0; j < nLevels; j++){
		for(int i = 0; i < 8; i++){
			Mesh copy = plane;
			//float d = (nLevels-j)/(float)nLevels*tp.width;
			float jf = 1.0f - j/(float)nLevels;
			float d = tp.width * tp.scale * (tp.a * jf*jf*jf + tp.b * jf*jf + tp.c * jf + tp.d * std::sqrt(-jf*jf + jf));
			if(tp.e == 0.0f) d = std::abs(d);
			else if(d < 0.0f) continue;
			copy.scale({d, 1.0f, d});
			copy.transform(Matrix4x4::rotationY(i/8.0f*2.0f*PI+j));
			copy.translate({0.0f, (float)(levelSpacing*j)+rand()%100/100.0f, 0.0f});
			tree.add(copy);
		}
	}
	float xzScale = 1.0f + rand() % (int)(tp.dev*100) / 100.0f;
	float yScale = 1.0f + rand() % (int)(tp.dev*100) / 100.0f;
	tree.scale({xzScale, yScale, xzScale});

	return tree;
}

Vegetation& Vegetation::instance(){
	return vegetationInstance;
}
