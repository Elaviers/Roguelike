#include "IO.hpp"
#include "BufferIterator.hpp"

enum class ModelVersion
{
	V1 = 1
};

constexpr byte CURRENT_MODEL_VERSION = (byte)ModelVersion::V1;

bool IO::WriteModelData(const char* filename, const ModelData& modelData)
{
	Buffer<byte> data;
	BufferIterator<byte> iterator(data);

	data.SetSize(
		1 +													//Version
		4 +													//Vert count
		4 * (3 + 3 + 2) * modelData.vertices.GetSize() +	//Vertices
		4 +													//Element count
		4 * modelData.elements.GetSize() +					//Elements
		4 * 3 +												//Bounds min
		4 * 3);												//Bounds max

	iterator.Write_byte(CURRENT_MODEL_VERSION);

	iterator.Write_uint32(modelData.vertices.GetSize());

	for (size_t i = 0; i < modelData.vertices.GetSize(); ++i)
	{
		const Vertex17F &v = modelData.vertices[i];

		iterator.Write_vector3(v.pos);
		iterator.Write_vector3(v.normal);
		iterator.Write_vector2(v.uvOffset);
	}

	iterator.Write_uint32(modelData.elements.GetSize());

	for (size_t i = 0; i < modelData.elements.GetSize(); ++i)
		iterator.Write_uint32(modelData.elements[i]);

	iterator.Write_vector3(modelData.bounds.min);
	iterator.Write_vector3(modelData.bounds.max);

	if (IO::WriteFile(filename, data.Data(), data.GetSize()))
		return true;

	return false;
}

ModelData IO::ReadModelData(const char* filename)
{
	ModelData data;

	Buffer<byte> buffer = IO::ReadFile(filename);
	
	if (buffer.GetSize() > 0)
	{
		BufferIterator<byte> iterator(buffer);

		ModelVersion version = (ModelVersion)iterator.Read_byte();

		if (version == ModelVersion::V1)
		{
			uint32 vertCount = iterator.Read_uint32();

			data.vertices.SetSize(vertCount);

			for (uint32 i = 0; i < vertCount; ++i)
			{
				data.vertices[i].pos = iterator.Read_vector3();
				data.vertices[i].normal = iterator.Read_vector3();
				data.vertices[i].uvOffset = iterator.Read_vector2();
			}

			uint32 elemCount = iterator.Read_uint32();

			data.elements.SetSize(elemCount);

			for (uint32 i = 0; i < elemCount; ++i)
				data.elements[i] = iterator.Read_uint32();

			data.bounds.min = iterator.Read_vector3();
			data.bounds.max = iterator.Read_vector3();
		}
	
		data.bounds.RecalculateSphereBounds();
	}

	return data;
}
