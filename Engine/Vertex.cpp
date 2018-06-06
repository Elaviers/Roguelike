#include "Vertex.h"

void Vertex14F::CalculateTangents(Vertex14F &vertex1, Vertex14F &vertex2, Vertex14F &vertex3)
{
	Vector3 edge1 = vertex2.pos - vertex1.pos;
	Vector2 deltaUV1 = vertex2.uv - vertex1.uv;
	Vector3 edge2 = vertex3.pos - vertex1.pos;
	Vector2 deltaUV2 = vertex3.uv - vertex1.uv;

	float f = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV2[0] * deltaUV1[1]);

	vertex1.tangent[0] = f * (deltaUV2[1] * edge1[0] - deltaUV1[1] * edge2[0]);
	vertex1.tangent[1] = f * (deltaUV2[1] * edge1[1] - deltaUV1[1] * edge2[1]);
	vertex1.tangent[2] = f * (deltaUV2[1] * edge1[2] - deltaUV1[1] * edge2[2]);
	vertex1.tangent.Normalise();
	vertex2.tangent = vertex1.tangent;
	vertex3.tangent = vertex1.tangent;

	vertex1.bitangent[0] = f * (-deltaUV2[0] * edge1[0] + deltaUV1[0] * edge2[0]);
	vertex1.bitangent[1] = f * (-deltaUV2[0] * edge1[1] + deltaUV1[0] * edge2[1]);
	vertex1.bitangent[2] = f * (-deltaUV2[0] * edge1[2] + deltaUV1[0] * edge2[2]);
	vertex1.bitangent.Normalise();
	vertex2.bitangent = vertex1.bitangent;
	vertex3.bitangent = vertex1.bitangent;
}
