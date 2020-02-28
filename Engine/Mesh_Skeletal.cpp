#include "Mesh_Skeletal.hpp"
#include "GLMeshRenderer.hpp"
#include "IO.hpp"

enum class EEFileVersion
{
	V1 = 1
};

constexpr byte CURRENT_FILE_VERSION = (byte)EEFileVersion::V1;

void Mesh_Skeletal::_ReadData(BufferReader<byte>& iterator)
{
	iterator.IncrementIndex(1); //Skip type

	byte version = iterator.Read_byte();

	if (version == (byte)EEFileVersion::V1)
	{
		byte bonesPerVertex = iterator.Read_byte();
		if (bonesPerVertex != VertexSkeletal::BONE_COUNT)
		{
			Debug::Error("I can't be bothered to load this mesh because it does not have 2 bones per vertex");
			return;
		}

		uint32 vertCount = iterator.Read_uint32();

		vertices.SetSize(vertCount);

		for (uint32 i = 0; i < vertCount; ++i)
		{
			vertices[i].pos = iterator.Read_vector3();
			vertices[i].normal = iterator.Read_vector3();
			vertices[i].uv = iterator.Read_vector2();

			for (int j = 0; j < bonesPerVertex; ++j)
			{
				vertices[i].boneIndices[j] = iterator.Read_uint32();
				vertices[i].boneWeights[j] = iterator.Read_float();
			}
		}

		uint32 elemCount = iterator.Read_uint32();

		elements.SetSize(elemCount);

		for (uint32 i = 0; i < elemCount; ++i)
		{
			elements[i] = iterator.Read_uint32();

			if ((i + 1) % 3 == 0)
				VertexSkeletal::CalculateTangents(vertices[elements[i - 2]], vertices[elements[i - 1]], vertices[elements[i]]);
		}

		uint32 jointCount = iterator.Read_uint32();

		for (size_t i = 0; i < jointCount; ++i)
		{
			Joint *parent = skeleton.GetJointWithID(iterator.Read_uint32());

			Joint* j = skeleton.CreateJoint(parent);
			j->name = iterator.Read_string();
			j->bindingMatrix.ReadFromBuffer(iterator);
		}

		bounds.min = iterator.Read_vector3();
		bounds.max = iterator.Read_vector3();
	}
}

void Mesh_Skeletal::_WriteData(BufferWriter<byte>& iterator) const
{
	uint32 jointCount = (uint32)skeleton.GetJointCount();

	Buffer<const Joint*> jointBuffer;
	jointBuffer.SetSize(jointCount);

	uint32 jointId = 0;

	size_t jointDataSize = 0;

	for (auto it = skeleton.FirstListElement(); it.IsValid(); ++it)
	{
		jointBuffer[jointId++] = &*it;

		//ParentID + name + binding
		jointDataSize += 4 + (it->name.GetLength() + 1) + (4 * 4 * 4);
	}

	iterator.EnsureSpace(
		1 +													//Type (Skeletal)
		1 +													//Version
		1 +													//Bone indices/weights per vertex (2)
		4 +													//Vert count
		(4 * (3+2+3+3+3+3) + ((4 + 4) * VertexSkeletal::BONE_COUNT)) * vertices.GetSize() +	//Vertices
		4 +													//Element count
		4 * elements.GetSize() +							//Elements
		4 +													//Joint count
		jointDataSize +										//Joints
		4 * 3 +												//Bounds min
		4 * 3);												//Bounds max

	iterator.Write_byte(ASSET_MESH_SKELETAL);
	iterator.Write_byte(CURRENT_FILE_VERSION);
	iterator.Write_byte(VertexSkeletal::BONE_COUNT);

	iterator.Write_uint32((uint32)vertices.GetSize());

	for (size_t i = 0; i < vertices.GetSize(); ++i)
	{
		const VertexSkeletal& v = vertices[i];

		iterator.Write_vector3(v.pos);
		iterator.Write_vector3(v.normal);
		iterator.Write_vector2(v.uv);
		
		for (int j = 0; j < VertexSkeletal::BONE_COUNT; ++j)
		{
			iterator.Write_uint32(v.boneIndices[j]);
			iterator.Write_float(v.boneWeights[j]);
		}
	}

	iterator.Write_uint32((uint32)elements.GetSize());

	for (size_t i = 0; i < elements.GetSize(); ++i)
		iterator.Write_uint32(elements[i]);

	iterator.Write_uint32((uint32)jointBuffer.GetSize());

	for (size_t i = 0; i < jointBuffer.GetSize(); ++i)
	{
		const Joint* joint = jointBuffer[i];

		size_t parentId = 0;
		for (size_t id = 0; id < jointBuffer.GetSize(); ++id)
		{
			if (joint->GetParent() == jointBuffer[id])
			{
				parentId = id;
				break;
			}
		}

		iterator.Write_uint32((uint32)parentId);
		iterator.Write_string(joint->name.GetData());
		joint->bindingMatrix.WriteToBuffer(iterator);
	}

	iterator.Write_vector3(bounds.min);
	iterator.Write_vector3(bounds.max);
}

void Mesh_Skeletal::CreateGLMeshRenderer(GLMeshRenderer& renderer) 
{ 
	renderer.Create(vertices.Data(), (GLsizei)vertices.GetSize(), elements.Data(), (GLsizei)elements.GetSize()); 
}
