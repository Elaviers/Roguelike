#pragma once
#include <ELMaths/Vector2.hpp>
#include <ELMaths/Vector3.hpp>

/*
	Vertex for static mesh

	POS			float*3
	UV			float*2
	COLOUR		float*3

	TANGENT		float*3
	BITANGENT	float*3
	NORMAL		float*3
	*/
struct Vertex17F
{
	Vector3 pos;
	Vector2 uv;
	Vector3 colour = Vector3(1.f, 1.f, 1.f);

	Vector3 tangent;
	Vector3 bitangent;
	Vector3 normal;

	static void CalculateTangents(Vertex17F &vertex1, Vertex17F &vertex2, Vertex17F &vertex3);
};

/*
	Vertex for skinned mesh

	POS			float*3
	UV			float*2
	COLOUR		float*3

	TANGENT		float*3
	BITANGENT	float*3
	NORMAL		float*3

	BONEINDICES	uint32*4
	BONEWEIGHTS	float*4
*/
struct VertexSkeletal
{
	Vector3 pos;
	Vector2 uv;
	Vector3 colour = Vector3(1.f, 1.f, 1.f);

	Vector3 tangent;
	Vector3 bitangent;
	Vector3 normal;

	//Cannot be more than 4
	constexpr const static int BONE_COUNT = 4;

	uint32 boneIndices[BONE_COUNT] = {};
	float boneWeights[BONE_COUNT] = {};

	static void CalculateTangents(VertexSkeletal& vertex1, VertexSkeletal& vertex2, VertexSkeletal& vertex3);
};

#define VERT14F_TRI(POS1, POS2, POS3, UV1, UV2, UV3, NORMAL) \
		{POS1, UV1, Vector3(1.f, 1.f, 1.f), Vector3(), Vector3(), NORMAL}, {POS2, UV2, Vector3(1.f, 1.f, 1.f), Vector3(), Vector3(), NORMAL}, {POS3, UV3, Vector3(1.f, 1.f, 1.f), Vector3(), Vector3(), NORMAL}
