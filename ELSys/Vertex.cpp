#include "Vertex.hpp"

template<typename T>
void _CalculateTangents(T& vertex1, T& vertex2, T& vertex3)
{
	Vector3 edge1 = vertex2.pos - vertex1.pos;
	Vector2 deltaUV1 = vertex2.uv - vertex1.uv;
	Vector3 edge2 = vertex3.pos - vertex1.pos;
	Vector2 deltaUV2 = vertex3.uv - vertex1.uv;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	vertex1.tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	vertex1.tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	vertex1.tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	vertex1.tangent.Normalise();
	vertex2.tangent = vertex1.tangent;
	vertex3.tangent = vertex1.tangent;

	vertex1.bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	vertex1.bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	vertex1.bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	vertex1.bitangent.Normalise();
	vertex2.bitangent = vertex1.bitangent;
	vertex3.bitangent = vertex1.bitangent;
}

void Vertex17F::CalculateTangents(Vertex17F &vertex1, Vertex17F &vertex2, Vertex17F &vertex3)
{
	_CalculateTangents(vertex1, vertex2, vertex3);
}

void VertexSkeletal::CalculateTangents(VertexSkeletal &vertex1, VertexSkeletal& vertex2, VertexSkeletal& vertex3)
{
	_CalculateTangents(vertex1, vertex2, vertex3);
}
