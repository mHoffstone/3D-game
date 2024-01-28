#ifndef DETAILCOLLECTION_H
#define DETAILCOLLECTION_H

#include "items.h"
#include "entity.h"

#include "engine/instancedrenderablemesh.h"

struct DetailDescriber{
	int levelOfDetail = 1;
	float heightMin = 0.0f;
	float heightMax = 9999999.0f;
	int tries = 0;
	int densityIndex = -1;
	float threshold = 0.5f;
	bool followNormal = false;
	std::vector<Item> items;
	Mesh mesh;
	bool instanceRendered = true;
	InstancedRenderableMesh irMesh;
	std::shared_ptr<Entity> entityTemplate = nullptr;
};

class DetailCollection{
public:
	DetailCollection();
	virtual ~DetailCollection();

	void create();
	void apply();

	bool hasCreated();
	bool hasApplied();

	int addDetailType(DetailDescriber describer);
	const DetailDescriber& getDescriber(int index) const;
	DetailDescriber& getDescriber(int index);
	int count() const;

	int addNoiseMap(std::unique_ptr<float[]> map);
	float getNoiseValue(int x, int y, int mapIndex) const;

	void render(Renderer& r);

private:
	std::vector<DetailDescriber> describers;
	std::vector<std::unique_ptr<float[]>> noiseMaps;

	bool m_hasCreated = false;
	bool m_hasApplied = false;
};

#endif // DETAILCOLLECTION_H
