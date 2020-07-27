#pragma once
#include "Mesh.hpp"
#include "Skeleton.hpp"
#include <ELCore/Buffer.hpp>
#include <ELSys/Vertex.hpp>

class Mesh_Skeletal : public Mesh
{
public:
	Mesh_Skeletal() {}
	virtual ~Mesh_Skeletal() {}

	Buffer<VertexSkeletal> vertices;
	Buffer<uint32> elements;
	Skeleton skeleton;

	virtual bool IsValid() const { return vertices.GetSize() != 0; }

	virtual void Read(ByteReader& data) override;
	virtual void Write(ByteWriter&) const override;

	virtual void UpdateRenderer() override;
};
