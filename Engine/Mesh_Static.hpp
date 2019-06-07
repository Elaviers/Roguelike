#pragma once
#include "Mesh.hpp"
#include "Buffer.hpp"
#include "Vertex.hpp"

class Mesh_Static : public Mesh
{
public:
	Mesh_Static() {}
	~Mesh_Static() {}

	Buffer<Vertex17F> vertices;
	Buffer<uint32> elements;

	virtual bool IsValid() const { return vertices.GetSize() != 0; }

	virtual void WriteData(BufferIterator<byte>&) const;
	virtual void ReadData(BufferIterator<byte>&);

	virtual void CreateGLMeshRenderer(GLMeshRenderer&);
};
