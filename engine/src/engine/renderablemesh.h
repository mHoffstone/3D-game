#ifndef RENDERABLEMESH_H
#define RENDERABLEMESH_H

#include "mesh.h"
#include "meshrenderer.h"
#include "objectlife.h"
#include "renderer.h"

#include <memory>

class RenderableMesh : public Mesh, protected MeshRenderer<vertex>, public ObjectLife{
public:
	RenderableMesh();
	virtual ~RenderableMesh();

	RenderableMesh& operator=(const Mesh& rhs);
	virtual void create() override;
	virtual void apply();
	virtual void render(Renderer& r);
	virtual void bind() override;

	virtual bool hasCreated();
	virtual bool hasApplied();

	virtual void destroy() override;

private:
	bool m_hasCreated = false;
	bool m_hasApplied = false;
};

#endif // RENDERABLEMESH_H
