#ifndef SPECIFICMESH_H
#define SPECIFICMESH_H

#include <vector>

template <class T>
class SpecificMesh{
public:
	std::vector<T> vertices;
	std::vector<unsigned int> indices;
};

#endif
