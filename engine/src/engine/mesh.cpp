#include "mesh.h"

#include <iostream>
#include <fstream>
#include <utility>
#include <cmath>

Mesh::Mesh(){

}

Mesh::~Mesh(){

}

/*const std::vector<vertex>& Mesh::getVertices() const{
	return vertices;
}
const std::vector<index>& Mesh::getIndices() const{
	return indices;
}*/

std::string getWord(const std::string& line, size_t& carret, char separator = ' '){
	size_t start = carret;
	carret = line.find(separator, carret);
	if(carret == std::string::npos) carret = line.length();
	carret++;
	//std::cout << line.substr(start, carret-1-start) << std::endl;
	return line.substr(start, carret-1-start);
}

Mesh Mesh::readObjFile(const std::string& path){
	Mesh mesh;

	std::vector<vec3f> points;
	std::vector<vec3f> normals;
	std::vector<vec2f> texCoords;

	int material = 0;

	std::ifstream reader;
	reader.open(path);
	std::string line;
	while(std::getline(reader, line)){
		size_t carret = 0;
		std::string command;
		command = getWord(line, carret);
		if(command == "v"){
			float values[3];
			values[0] = std::stof(getWord(line, carret));
			values[1] = std::stof(getWord(line, carret));
			values[2] = std::stof(getWord(line, carret));
			points.push_back(vec3f{values[0], values[1], values[2]});
		}
		if(command == "vn"){
			float values[3];
			values[0] = std::stof(getWord(line, carret));
			values[1] = std::stof(getWord(line, carret));
			values[2] = std::stof(getWord(line, carret));
			normals.push_back(vec3f{values[0], values[1], values[2]});
		}
		if(command == "vt"){
			float values[3];
			values[0] = std::stof(getWord(line, carret));
			values[1] = std::stof(getWord(line, carret));
			texCoords.push_back(vec2f{values[0], values[1]});
		}
		if(command == "f"){
			std::string vertexString;
			for(int i = 0; i < 3; i++){
				vertexString = getWord(line, carret);
				size_t subcarret = 0;
				int posi = std::stoi(getWord(vertexString, subcarret, '/'))-1;
				int texi = std::stoi(getWord(vertexString, subcarret, '/'))-1;
				int nori = std::stoi(getWord(vertexString, subcarret, '/'))-1;
				mesh.vertices.push_back(vertex{points.at(posi), normals.at(nori), texCoords.at(texi), (float)material});
				mesh.indices.push_back(mesh.vertices.size()-1);
			}
		}
		if(command == "usemtl"){
			std::string mtl = getWord(line, carret);
			if(util::isInt(mtl)){
				material = std::stoi(mtl);
			}
			else{
				material = 0;
			}
		}
	}

	return mesh;
}

Mesh& Mesh::add(const Mesh& m){
	/*const std::vector<index> iarr = m.getIndices();
	for(int i = 0; i < (int)iarr.size(); i++){
		indices.push_back(iarr.at(i) + vertices.size());
	}
	//indices.insert(indices.end(), iarr.begin(), iarr.end());

	const std::vector<vertex> varr = m.getVertices();
	vertices.insert(vertices.end(), varr.begin(), varr.end());*/

	for(int i = 0; i < (int)m.indices.size(); i++){
		indices.push_back(m.indices.at(i) + vertices.size());
	}
	vertices.insert(vertices.end(), m.vertices.begin(), m.vertices.end());
	return *this;
}
Mesh& Mesh::clear(){
	indices.clear();
	vertices.clear();
	return *this;
}

Mesh& Mesh::translate(vec3f pos){
	for(int i = 0; i < (int)vertices.size(); i++){
		vertices[i].pos += pos;
	}
	return *this;
}
Mesh& Mesh::scale(vec3f size){
	for(int i = 0; i < (int)vertices.size(); i++){
		vertices[i].pos *= size;
	}
	return *this;
}
Mesh& Mesh::scale(float size){
	return scale(vec3f{size,size,size});
}

Mesh& Mesh::textureRepeat(int times){
	for(int i = 0; i < (int)vertices.size(); i++){
		vertices[i].texCoords *= times;
	}
	return *this;
}

Mesh& Mesh::transform(Matrix4x4 matrix){
	for(auto& a : vertices){
		a.pos = matrix*a.pos;
		a.normal = matrix*a.normal;
	}
	return *this;
}

Mesh& Mesh::setTexture(int texture, int start, int end){
	for(int i = start; i < end; i++){
		vertices.at(i).texIndex = texture;
	}
	return *this;
}
Mesh& Mesh::setTexture(int texture){
	setTexture(texture, 0, vertices.size());
	return *this;
}
Mesh& Mesh::setNormal(vec3f normal){
	for(auto& a : vertices){
		a.normal = normal;
	}
	return *this;
}

Mesh Mesh::createSphere(const vec3f& pos, float radius){
	Mesh sphere;
	int n = 32, m = 16;
	for(int j = 0; j < m; j++){
		for(int i = 0; i < n+1; i++){
			float h = (2*j-(m-1))/float(m-1);
			float d = std::sqrt(1-h*h);
			vec3f p = vec3f{std::cos(i*2*PI/n)*d, h, std::sin(i*2*PI/n)*d} * radius;
			sphere.vertices.push_back({pos+p, p, {i/(float)n, j/(float)m}, 0});
		}
	}
	for(int j = 0; j < m-1; j++){
		for(int i = 0; i < n; i++){
			sphere.indices.push_back(j*n+j + i);
			sphere.indices.push_back(j*n+j + i+1);
			sphere.indices.push_back(j*n+j + i+1 + n+1);

			sphere.indices.push_back(j*n+j + i);
			sphere.indices.push_back(j*n+j + i+1 + n+1);
			sphere.indices.push_back(j*n+j + i + n+1);

			// 			sphere.indices.push_back(j*n + i);
			// 			sphere.indices.push_back(j*n + (i+1)%n);
			// 			sphere.indices.push_back(j*n + (i+1)%n + n);
			//
			// 			sphere.indices.push_back(j*n + i);
			// 			sphere.indices.push_back(j*n + (i+1)%n + n);
			// 			sphere.indices.push_back(j*n + i + n);
		}
	}
	return sphere;
}
Mesh Mesh::createCylinder(const vec3f& pos, float radius, float height, MeshMode mode){
	if(mode == MeshMode::STRETCH){
		Mesh cylinder;

		int n = 8;
		for(int i = 0; i < n; i++){
			vec3f p = {std::cos(i*2*PI/n), 0.0f, std::sin(i*2*PI/n)};
			cylinder.vertices.push_back({p*radius+pos, 							 p, {i/(float)n, 0.0f}, 0.0f});
			cylinder.vertices.push_back({p*radius+pos+vec3f{0.0f, height, 0.0f}, p, {i/(float)n, 1.0f}, 0.0f});
		}
		for(int i = 0; i < 2*n; i += 2){
			cylinder.indices.push_back(i);
			cylinder.indices.push_back((i+2)%(2*n));
			cylinder.indices.push_back((i+3)%(2*n));
			cylinder.indices.push_back(i);
			cylinder.indices.push_back((i+3)%(2*n));
			cylinder.indices.push_back(i+1);
		}
		return cylinder;
	}
	else{
		Mesh cylinder;
		int nParts = height/(2*PI*radius);
		for(int i = 0; i < nParts; i++){
			cylinder.add(createCylinder(pos + vec3f{0.0f, i*height/(float)nParts, 0.0f} , radius, height/(float)nParts, MeshMode::STRETCH));
		}
		return cylinder;
	}
}
Mesh Mesh::createCuboid(const vec3f& pos, const vec3f& size){
	Mesh cuboid;
	cuboid.vertices.push_back({{0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, 0.0f});
	cuboid.vertices.push_back({{1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, 0.0f});
	cuboid.vertices.push_back({{1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, 0.0f});
	cuboid.vertices.push_back({{0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, 0.0f});

	cuboid.vertices.push_back({{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, 0.0f});
	cuboid.vertices.push_back({{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, 0.0f});
	cuboid.vertices.push_back({{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, 0.0f});
	cuboid.vertices.push_back({{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, 0.0f});

	cuboid.vertices.push_back({{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, 0.0f});
	cuboid.vertices.push_back({{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, 0.0f});
	cuboid.vertices.push_back({{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, 0.0f});
	cuboid.vertices.push_back({{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, 0.0f});

	cuboid.vertices.push_back({{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, 0.0f});
	cuboid.vertices.push_back({{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, 0.0f});
	cuboid.vertices.push_back({{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, 0.0f});
	cuboid.vertices.push_back({{0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, 0.0f});

	cuboid.vertices.push_back({{0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f});
	cuboid.vertices.push_back({{0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, 0.0f});
	cuboid.vertices.push_back({{0.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f});
	cuboid.vertices.push_back({{0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, 0.0f});

	cuboid.vertices.push_back({{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f});
	cuboid.vertices.push_back({{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, 0.0f});
	cuboid.vertices.push_back({{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, 0.0f});
	cuboid.vertices.push_back({{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, 0.0f});

	for(auto& v : cuboid.vertices){
		v.pos *= size;
		v.pos += pos;
	}

	for(int i = 0; i < 6; i++){
		cuboid.indices.push_back(4*i);
		cuboid.indices.push_back(4*i+1);
		cuboid.indices.push_back(4*i+2);
		cuboid.indices.push_back(4*i);
		cuboid.indices.push_back(4*i+2);
		cuboid.indices.push_back(4*i+3);
	}
	return cuboid;
}
Mesh Mesh::createXYPlane(const vec3f& pos, const vec2f& size){
	Mesh m;
	m.vertices = {
		{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, 0.0f},
		{{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, 0.0f},
		{{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, 0.0f},
		{{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, 0.0f},
	};
	m.indices = {0, 1, 2, 0, 2, 3};
	m.translate(pos);
	m.scale({size.x, size.y, 1.0f});
	return m;
}
Mesh Mesh::createXZPlane(const vec3f& pos, const vec2f& size){
	Mesh m;
	m.vertices = {
		{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, 0.0f},
		{{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, 0.0f},
		{{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, 0.0f},
		{{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, 0.0f},
	};
	m.indices = {0, 1, 2, 0, 2, 3};
	m.translate(pos);
	m.scale({size.x, 1.0f, size.y});
	return m;
}
Mesh Mesh::createCone(const vec3f& pos, float radius, float height, MeshMode mode){
	auto createSubCone = [pos, radius, height](int nPart, int nParts){
		Mesh m;
		int n = 8;
		vec3f hOffset = {0.0f, height/(float)nParts, 0.0f};
		for(int i = 0; i < n+1; i++){
			vec3f p = {std::cos(i*2*PI/n), 0.0f, std::sin(i*2*PI/n)};
			m.vertices.push_back({p*radius*(1.0f-nPart/(float)nParts)+pos+hOffset*nPart, 			p, {i/(float)n, 0.0f}, 0.0f});
			m.vertices.push_back({p*radius*(1.0f-(nPart+1)/(float)nParts)+pos+hOffset*(nPart+1),	p, {i/(float)n, 1.0f}, 0.0f});
		}
		for(int i = 0; i < 2*n; i += 2){
			m.indices.push_back(i);
			m.indices.push_back((i+2));
			m.indices.push_back((i+3));
			m.indices.push_back(i);
			m.indices.push_back((i+3));
			m.indices.push_back(i+1);
		}
		return m;
	};
	if(mode == MeshMode::STRETCH) return createSubCone(0, 1);
	else{
		int nParts = height / (2*PI*radius);
		Mesh m;
		for(int i = 0; i < nParts; i++){
			m.add(createSubCone(i, nParts));
		}
		return m;
	}
}

