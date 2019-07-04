#pragma once
#include "Asset.hpp"
#include "Bounds.hpp"

class GLMeshRenderer;

class Mesh : public Asset
{
protected:
	Mesh() {}

public:
	Bounds bounds;

	virtual ~Mesh() {}

	static Mesh* FromData(Buffer<byte>&);

	virtual bool IsValid() const { return false; }

	virtual void CreateGLMeshRenderer(GLMeshRenderer&) {}
};
