#pragma once
#include "Mesh.hpp"
#include "Buffer.hpp"
#include "Vertex.hpp"
#include "Skeleton.hpp"

class Mesh_Skeletal : public Mesh
{
public:
	Mesh_Skeletal() {}
	virtual ~Mesh_Skeletal() {}

	Buffer<Vertex19F2I> vertices;
	Buffer<uint32> elements;
	Skeleton skeleton;

	virtual bool IsValid() const { return vertices.GetSize() != 0; }

	virtual void WriteData(BufferIterator<byte>&) const;
	virtual void ReadData(BufferIterator<byte>&);

	virtual void CreateGLMeshRenderer(GLMeshRenderer&);
};
