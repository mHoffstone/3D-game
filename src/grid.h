#ifndef GRID_H
#define GRID_H

#include "engine/renderer.h"
#include "engine/program.h"
#include "engine/renderablemesh.h"
#include "engine/uniformmatrix4x4.h"
#include "engine/uniformvec3f.h"

class Grid{
public:
	Grid(){}
	Grid(int x, int y, int width, int height);
	virtual ~Grid();

	static void init(Renderer& r);

	void generate(int x, int y, int width, int height);

	void create();
	void apply();

	void render(Renderer& r);

	float getHeightAt(float x, float y);
	void setHeightAt(int x, int y, float height);
	vec3f getNormalAt(float x, float y);

	void recalculate();

	int getWidth();
	int getHeight();

	int getX();
	int getY();

private:
	int x, y;
	int width, height;
	std::unique_ptr<float[]> heights = nullptr;
	std::unique_ptr<vec3f[]> normals = nullptr;

	std::unique_ptr<float[]> shaderData;

	unsigned int ssbo;
	bool hasCreated = false;
	bool hasApplied = false;

	static Program program;
	static Uniform widthUniform;
	static Uniform positionUniform;
	static int rendererID;
	static RenderableMesh gridPiece;

	void generateGrid();
};

#endif // GRID_H
