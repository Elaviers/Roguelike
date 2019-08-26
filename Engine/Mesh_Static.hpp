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
	Mesh_Static(const Buffer<Vertex17F>& vertices = Buffer<Vertex17F>(), const Buffer<uint32>& elements = Buffer<uint32>()) : vertices(vertices), elements(elements) 
	{}

	virtual ~Mesh_Static() {}

	Buffer<Vertex17F> vertices;
	Buffer<uint32> elements;

	virtual bool IsValid() const { return vertices.GetSize() != 0; }

	virtual void CreateGLMeshRenderer(GLMeshRenderer&);
};
