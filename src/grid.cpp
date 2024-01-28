#include "grid.h"

#include "noisemap.h"

#include <GL/glew.h>

//#include <cstdint>

Program Grid::program;
Uniform Grid::widthUniform;
Uniform Grid::positionUniform;
int Grid::rendererID = -1;
RenderableMesh Grid::gridPiece;

void Grid::generateGrid(){
	for(int i = 0; i < width+1; i++){
		for(int j = 0; j < height+1; j++){
			int xp = i+x;
			int yp = j+y;
			//data[(j*(width+1)+i)*4] = 1.0f*noise(0, xp/3.0f, yp/3.0f) + 4.0f*noise(1, xp/15.0f, yp/15.0f) + 16.0f*noise(2, xp/100.0f, yp/100.0f) + 100.0f*noise(3, xp/1000.0f, yp/1000.0f);
			float mountaininess = 100.0f * NoiseMap::valueAt(xp/100.0f, yp/100.0f, 1);
			float hilliness = 10.0f * NoiseMap::valueAt(xp/50.0f, yp/50.0f, 2);
			float roughness = 1.0f * NoiseMap::valueAt(xp/100.0f, yp/100.0f, 3);
			float v =
				mountaininess*NoiseMap::valueAt(xp/200.0f, yp/200.0f, 0) - 25.0f +
				hilliness*NoiseMap::valueAt(xp/20.0f, yp/20.0f, 0) +
				roughness*NoiseMap::valueAt(xp/2.3f, yp/2.3f, 0);

			shaderData[(j*(width+1)+i)*4] = v;
		}
	}
}

Grid::~Grid(){
	//util::log("Grid deleted :(");
	//delete[] shaderData;
}

void Grid::init(Renderer& r){
	program.createProgram();
	Shader vertexShader(GL_VERTEX_SHADER, util::readFile("res/shaders/grid_vertex.glsl"));
	Shader fragmentShader(GL_FRAGMENT_SHADER, util::readFile("res/shaders/grid_fragment.glsl"));
	program.setShaders({&vertexShader, &fragmentShader});

	widthUniform.setLocation(program, "u_width");
	positionUniform.setLocation(program, "u_position");

	rendererID = r.addProgram(program);
	gridPiece.create();
	gridPiece.vertices = {
		{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, 0.0f},
		{{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, 0.0f},
		{{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, 0.0f},
		{{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, 0.0f},
	};
	gridPiece.indices = {0, 1, 2, 0, 2, 3};
	gridPiece.apply();
}

void Grid::generate(int x, int y, int width, int height){
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	heights = std::unique_ptr<float[]>(new float[(width+1)*(height+1)]);
	normals = std::unique_ptr<vec3f[]>(new vec3f[(width+1)*(height+1)]);

	int size = (width+1)*(height+1)*4;
	shaderData = std::unique_ptr<float[]>(new float[size]);
	generateGrid();
	recalculate();
}

void Grid::create(){
	if(hasCreated) return;
	glBindVertexArray(0);
	glGenBuffers(1, &ssbo);
	hasCreated = true;
}
void Grid::apply(){
	if(!hasCreated) throw std::runtime_error("Grid was not created!");
	if(hasApplied) return;
	glBindVertexArray(0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	int size = (width+1)*(height+1)*4;
	glBufferData(GL_SHADER_STORAGE_BUFFER, size*sizeof(float), shaderData.get(), GL_STATIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	hasApplied = true;
}


void Grid::render(Renderer& r){
	if(!hasCreated) throw std::runtime_error("Grid was not created!");
	//if(!hasApplied) throw std::runtime_error("Grid was not applied!");
	apply();
	r.useProgram(rendererID);
	gridPiece.bind();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glUniform1i(widthUniform.getLocation(), width);
	glUniform2f(positionUniform.getLocation(), x, y);
	glDrawElementsInstanced(GL_TRIANGLES, gridPiece.indices.size(), GL_UNSIGNED_INT, 0, width*height);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

float Grid::getHeightAt(float x, float y){
	if(x < 0.0f || y < 0.0f || x > width+1.0f || y > height+1.0f) return 0;
	int xl = x, yl = y;
	float
		v00=heights[(yl  )*(width+1)+(xl  )],
		v10=heights[(yl  )*(width+1)+(xl+1)],
		v11=heights[(yl+1)*(width+1)+(xl+1)],
		v01=heights[(yl+1)*(width+1)+(xl  )];
	return util::berp(v00, v01, v10, v11, x-xl, y-yl, util::lerp);
	//return heights[y*(width+1)+x];
}
void Grid::setHeightAt(int x, int y, float height){
	shaderData[(y*(width+1)+x)*4] = height;
	recalculate();
}

vec3f Grid::getNormalAt(float x, float y){
	if(x < 0.0f || y < 0.0f || x > width+1.0f || y > height+1.0f) return vec3f{0, 1, 0};
	int xl = x, yl = y;
	vec3f
	v00=normals[(yl  )*(width+1)+(xl  )],
	v10=normals[(yl  )*(width+1)+(xl+1)],
	v11=normals[(yl+1)*(width+1)+(xl+1)],
	v01=normals[(yl+1)*(width+1)+(xl  )];
	return util::berp(v00, v01, v10, v11, x-xl, y-yl, util::lerp);
}

void Grid::recalculate(){
	for(int i = 0; i < width+1; i++){
		for(int j = 0; j < height; j++){
			int k = (j*(width+1)+i)*4;
			vec3f v = {(float)i,   shaderData[k], (float)j};
			vec3f x = {(float)i+1, shaderData[(    j*(width+1)+(i+1))*4], float(j)};
			vec3f y = {(float)i,   shaderData[((j+1)*(width+1)+i)*4],     float(j)+1};
			vec3f n = (y-v).normalized().cross((x-v).normalized());
			shaderData[k+1] = n.x;
			shaderData[k+2] = n.y;
			shaderData[k+3] = n.z;
		}
		shaderData[(height*(width+1)+i)*4+1] = 0.0f;
		shaderData[(height*(width+1)+i)*4+2] = 1.0f;
		shaderData[(height*(width+1)+i)*4+3] = 0.0f;
	}
	for(int j = 0; j < height; j++){
		shaderData[(j*(width+1)+width)*4+1] = 0.0f;
		shaderData[(j*(width+1)+width)*4+2] = 1.0f;
		shaderData[(j*(width+1)+width)*4+3] = 0.0f;
	}

	for(int i = 0; i < (width+1)*(height+1); i++) heights[i] = shaderData[4*i];
	for(int i = 0; i < (width+1)*(height+1); i++){
		normals[i].x = shaderData[4*i+1];
		normals[i].y = shaderData[4*i+2];
		normals[i].z = shaderData[4*i+3];
	}
	hasApplied = false;
}


int Grid::getWidth(){
	return width;
}
int Grid::getHeight(){
	return height;
}

int Grid::getX(){
	return x;
}
int Grid::getY(){
	return y;
}


