#pragma once
#include "Bounds.hpp"
#include "BufferIterator.hpp"

class GLMeshRenderer;

class Mesh
{
protected:
	Mesh() {}

public:
	Bounds bounds;

	virtual ~Mesh() {}

	virtual bool IsValid() const { return false; }

	virtual void WriteData(BufferIterator<byte>&) const {}
	virtual void ReadData(BufferIterator<byte>&) {}

	virtual void CreateGLMeshRenderer(GLMeshRenderer&) {}
};
