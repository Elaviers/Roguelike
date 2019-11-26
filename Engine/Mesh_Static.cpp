#include "Mesh_Static.hpp"
#include "GLMeshRenderer.hpp"
#include "IO.hpp"

enum class FileVersion
{
	V1 = 1
};

constexpr byte CURRENT_FILE_VERSION = (byte)FileVersion::V1;

void Mesh_Static::_ReadData(BufferReader<byte>& iterator)
{
	iterator.IncrementIndex(1); //Skip type

	byte version = iterator.Read_byte();

	if (version == (byte)FileVersion::V1)
	{
		uint32 vertCount = iterator.Read_uint32();

		vertices.SetSize(vertCount);

		for (uint32 i = 0; i < vertCount; ++i)
		{
			vertices[i].pos = iterator.Read_vector3();
			vertices[i].normal = iterator.Read_vector3();
			vertices[i].uv = iterator.Read_vector2();

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

		bounds.min = iterator.Read_vector3();
		bounds.max = iterator.Read_vector3();
	}
}

void Mesh_Static::_WriteData(BufferWriter<byte>& iterator) const
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

		iterator.Write_vector3(v.pos);
		iterator.Write_vector3(v.normal);
		iterator.Write_vector2(v.uv);
	}

	iterator.Write_uint32((uint32)elements.GetSize());

	for (size_t i = 0; i < elements.GetSize(); ++i)
		iterator.Write_uint32(elements[i]);

	iterator.Write_vector3(bounds.min);
	iterator.Write_vector3(bounds.max);
}

void Mesh_Static::CreateGLMeshRenderer(GLMeshRenderer& renderer) 
{ 
	renderer.Create(vertices.Data(), (GLsizei)vertices.GetSize(), elements.Data(), (GLsizei)elements.GetSize());
}
