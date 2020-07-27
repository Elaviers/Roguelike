#include "Mesh_Static.hpp"
#include "IO.hpp"

Mesh_Static::Mesh_Static(const Buffer<Vertex17F>& vertices, const Buffer<uint32>& elements) : vertices(vertices), elements(elements)
{
}

void Mesh_Static::UpdateRenderer()
{
	_renderer.GL().Create(vertices.Data(), (GLsizei)vertices.GetSize(), elements.Data(), (GLsizei)elements.GetSize());
}

enum class EEFileVersion
{
	V1 = 1
};

constexpr byte CURRENT_FILE_VERSION = (byte)EEFileVersion::V1;

void Mesh_Static::Read(ByteReader& iterator)
{
	iterator.IncrementIndex(1); //Skip type

	byte version = iterator.Read_byte();

	if (version == (byte)EEFileVersion::V1)
	{
		uint32 vertCount = iterator.Read_uint32();

		vertices.SetSize(vertCount);

		for (uint32 i = 0; i < vertCount; ++i)
		{
			vertices[i].pos.Read(iterator);
			vertices[i].normal.Read(iterator);
			vertices[i].uv.Read(iterator);

			if ((i + 1) % 3 == 0)
				Vertex17F::CalculateTangents(vertices[i - 2], vertices[i - 1], vertices[i - 2]);
		}

		uint32 elemCount = iterator.Read_uint32();

		elements.SetSize(elemCount);

		for (uint32 i = 0; i < elemCount; ++i)
		{
			elements[i] = iterator.Read_uint32();

			if ((i + 1) % 3 == 0)
				Vertex17F::CalculateTangents(vertices[elements[i - 2]], vertices[elements[i - 1]], vertices[elements[i]]);
		}

		bounds.min.Read(iterator);
		bounds.max.Read(iterator);

		UpdateRenderer();
	}
}

void Mesh_Static::Write(ByteWriter& iterator) const
{
	iterator.EnsureSpace(
		1 +													//Type (Static)
		1 +													//Version
		4 +													//Vert count
		(4 * (3+3+2)) * vertices.GetSize() +				//Vertices
		4 +													//Element count
		4 * elements.GetSize() +							//Elements
		4 * 3 +												//Bounds min
		4 * 3);												//Bounds max

	iterator.Write_byte(ASSET_MESH_STATIC);
	iterator.Write_byte(CURRENT_FILE_VERSION);

	iterator.Write_uint32((uint32)vertices.GetSize());

	for (size_t i = 0; i < vertices.GetSize(); ++i)
	{
		const Vertex17F& v = vertices[i];

		v.pos.Write(iterator);
		v.normal.Write(iterator);
		v.uv.Write(iterator);
	}

	iterator.Write_uint32((uint32)elements.GetSize());

	for (size_t i = 0; i < elements.GetSize(); ++i)
		iterator.Write_uint32(elements[i]);

	bounds.min.Write(iterator);
	bounds.max.Write(iterator);
}
