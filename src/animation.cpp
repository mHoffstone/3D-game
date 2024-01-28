#include "animation.h"

#include <cmath>

Program Animation::program;
int Animation::rendererID;
Uniform Animation::displacements;

Animation::Animation(){

}

Animation::~Animation(){

}

void Animation::init(Renderer& r){
	program.createProgram();
	Shader vertexShader(GL_VERTEX_SHADER, util::readFile("res/shaders/animation_vertex.glsl"));
	Shader fragmentShader(GL_FRAGMENT_SHADER, util::readFile("res/shaders/animation_fragment.glsl"));
	program.setShaders({&vertexShader, &fragmentShader});

	rendererID = r.addProgram(program);

	displacements.setLocation(program, "u_displacements");
}


void Animation::setBaseMesh(const Mesh& m){
	mesh.vertices.reserve(m.vertices.size());
	mesh.indices.reserve(m.indices.size());
	for(auto it = m.vertices.begin(); it != m.vertices.end(); it++){
		AnimatedVertex v;
		v.pos = it->pos;
		v.normal = it->normal;
		v.texCoords = it->texCoords;
		v.texIndex = it->texIndex;
		v.boneId = 0;
		v.weight = 1.0f;
		mesh.vertices.push_back(v);
	}
	for(auto it = m.indices.begin(); it != m.indices.end(); it++){
		mesh.indices.push_back(*it);
	}
	util::logt("v", mesh.vertices.size());
	util::logt("i", mesh.indices.size());
}

int Animation::addPart(const Mesh& m){
	util::log("v", m.vertices.size());
	util::log("i", m.indices.size());
	if(m.indices.size() != mesh.indices.size() || m.vertices.size() != mesh.vertices.size()){
		throw std::runtime_error("Meshes had different sizes");
	}
	for(unsigned int i = 0; i < m.vertices.size(); i++){
		float delta = (m.vertices[i].pos - mesh.vertices[i].pos).length();
		if(delta > 0.01f){
			mesh.vertices[i].boneId = currentBone;
			mesh.vertices[i].weight = delta;
			util::log("id", mesh.vertices[i].boneId);
			util::log("le", mesh.vertices[i].weight);
		}
	}
	return currentBone++;
}

void Animation::create(){
	meshRenderer.create();

	glBindBuffer(GL_ARRAY_BUFFER, meshRenderer.vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)(1 * sizeof(vec3f)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)(2 * sizeof(vec3f)));
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)(2 * sizeof(vec3f) + 1 * sizeof(vec2f)));
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)(2 * sizeof(vec3f) + 1 * sizeof(vec2f) + 1 * sizeof(float)));
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(AnimatedVertex), (void*)(2 * sizeof(vec3f) + 1 * sizeof(vec2f) + 2 * sizeof(float)));
}

void Animation::apply(){
	meshRenderer.apply(mesh);
}

void Animation::render(Renderer& r){
	static float t = 0.0f;

	r.useProgram(rendererID);
	vec3f v[8] = {
		pos,
		pos + vec3f{ 0.5f,  0.5f*std::cos(2*PI*t),  0.5f*std::sin(2*PI*t)},
		pos + vec3f{-0.5f, -0.5f*std::cos(2*PI*t), -0.5f*std::sin(2*PI*t)}
	};
// 	vec3f v[8] = {
// 		{0.0f,30.0f,0.0f},
// 		{0.5f, 30.0f+0.5f*std::cos(2*PI*t), 0.5f*std::sin(2*PI*t)},
// 		{-0.5f, 30.0f-0.5f*std::cos(2*PI*t), -0.5f*std::sin(2*PI*t)}
// 	};
	glUniform3fv(displacements.getLocation(), 8, (float*)&v);
	meshRenderer.render(r, mesh);

	t += 1/60.0f;
}



