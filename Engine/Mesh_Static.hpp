#pragma once
#include "Mesh.hpp"
#include "Buffer.hpp"
#include "Vertex.hpp"

class Mesh_Static : public Mesh
{
protected:
	virtual void _ReadData(BufferReader<byte>& data) override;
	virtual void _WriteData(BufferWriter<byte>& data) const override;

public:
	Mesh_Static() {}
	~Mesh_Static() {}

	Buffer<Vertex17F> vertices;
	Buffer<uint32> elements;

	virtual bool IsValid() const { return vertices.GetSize() != 0; }

	virtual void CreateGLMeshRenderer(GLMeshRenderer&);
};
