#ifndef MESH_H
#define MESH_H

#include "util.h"

#include <vector>
#include <string>

#include "matrix4x4.h"
#include "specificmesh.h"

enum class MeshMode{
	SUBDIVIDE, STRETCH
};

class Mesh : public SpecificMesh<vertex>{
	public:
		Mesh();
		virtual ~Mesh();

		static Mesh readObjFile(const std::string& path);

		Mesh& add(const Mesh& m);
		Mesh& clear();
		Mesh& translate(vec3f pos);
		Mesh& scale(vec3f size);
		Mesh& scale(float size);
		Mesh& textureRepeat(int times);
		Mesh& transform(Matrix4x4 matrix);

		Mesh& setTexture(int texture, int start, int end);
		Mesh& setTexture(int texture);

		Mesh& setNormal(vec3f normal);

		static Mesh createSphere(const vec3f& pos, float radius);
		static Mesh createCylinder(const vec3f& pos, float radius, float height, MeshMode mode);
		static Mesh createCuboid(const vec3f& pos, const vec3f& size);
		static Mesh createXYPlane(const vec3f& pos, const vec2f& size);
		static Mesh createXZPlane(const vec3f& pos, const vec2f& size);
		static Mesh createCone(const vec3f& pos, float radius, float height, MeshMode mode);

		/*const std::vector<vertex>& getVertices() const;
		const std::vector<index>& getIndices() const;*/
};

#endif // MESH_H
