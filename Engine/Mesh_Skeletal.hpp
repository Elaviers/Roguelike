#pragma once
#include "Mesh.hpp"
#include "Buffer.hpp"
#include "Vertex.hpp"
#include "Skeleton.hpp"

class Mesh_Skeletal : public Mesh
{
protected:
	virtual void _ReadData(BufferReader<byte>& data) override;
	virtual void _WriteData(BufferWriter<byte>&) const override;

public:
	Mesh_Skeletal() {}
	virtual ~Mesh_Skeletal() {}

	Buffer<Vertex19F2I> vertices;
	Buffer<uint32> elements;
	Skeleton skeleton;

	virtual bool IsValid() const { return vertices.GetSize() != 0; }

	virtual void CreateGLMeshRenderer(GLMeshRenderer &out);
};
