#pragma once
#include "Mesh.hpp"
#include <ELCore/Buffer.hpp>
#include <ELSys/Vertex.hpp>

class Mesh_Static : public Mesh
{
public:
	Mesh_Static(const Buffer<Vertex17F>& vertices = Buffer<Vertex17F>(), const Buffer<uint32>& elements = Buffer<uint32>());
	virtual ~Mesh_Static() {}

	Buffer<Vertex17F> vertices;
	Buffer<uint32> elements;

	virtual bool IsValid() const { return vertices.GetSize() != 0; }

	virtual void Read(ByteReader& data) override;
	virtual void Write(ByteWriter& data) const override;

	virtual void UpdateRenderer() override;
};
