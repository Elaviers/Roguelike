#include "IO.hpp"
#include "Buffer.hpp"
#include "Debug.hpp"
#include "Utilities.hpp"
#include <stdlib.h>

struct OBJVertexDef
{
	int pos_index;
	int normal_index;
	int uv_index;
};

OBJVertexDef ParseOBJVertex(const String &term)
{
	int uvStringIndex = 0;
	int normalStringIndex = 0;

	for (unsigned int i = 0; i < term.GetLength(); ++i)
	{
		if (term[i] == '/')
		{
			i++;

			if (term[i] != '/')
				if (uvStringIndex == 0)
					uvStringIndex = i;
				else
				{
					normalStringIndex = i;
					break;
				}
			else
			{
				normalStringIndex = i + 1;
				break;
			}
		}

	}

	OBJVertexDef result = {};
	result.pos_index = atoi(term.GetData());
	if (uvStringIndex)
		result.uv_index = atoi(term.GetData() + uvStringIndex);
	if (normalStringIndex)
		result.normal_index = atoi(term.GetData() + normalStringIndex);

	return result;
}

template<typename T>
inline const uint32 OBJIndexToCIndex(const Buffer<T> &buf, size_t objIndex)
{
	if (objIndex == 0)
		return 0;

	if (objIndex < 0)
		return buf.GetSize() + objIndex;

	return objIndex - 1;
}

uint32 GetVertexIndex(Buffer<Vertex17F> &vertices, uint32 &vertexCount, const Buffer<Vector3> &positions, const Buffer<Vector3> &normals, const Buffer<Vector2> &uvs, const char *vertex)
{
	OBJVertexDef objVertex = ParseOBJVertex(vertex);
	const Vector3 &pos = positions[OBJIndexToCIndex(vertices, objVertex.pos_index)];
	const Vector3 &normal = normals[OBJIndexToCIndex(normals, objVertex.normal_index)];
	const Vector2 &uvOffset = uvs[OBJIndexToCIndex(uvs, objVertex.uv_index)];

	uint32 i = 0;
	for (; i < vertexCount; ++i)
		if (vertices[i].pos == pos && vertices[i].normal == normal && vertices[i].uvOffset == uvOffset)
			return i;

	if (i == vertices.GetSize())
		vertices.Append(1);

	vertices[i].pos = pos;
	vertices[i].normal = normal;
	vertices[i].uvOffset = uvOffset;

	vertexCount++;
	return i;
}

ModelData IO::ReadOBJFile(const char *filename)
{
	const float scale = 1;

	ModelData data;

	String objSrc = IO::ReadFileString(filename);
	if (objSrc.GetLength())
	{
		Buffer<Vector3> positions;
		Buffer<Vector3> normals;
		Buffer<Vector2> uvs;

		uint32 vertexCount = 0;

		Buffer<String> lines = objSrc.Split("\r\n");

		for (uint32 i = 0; i < lines.GetSize(); ++i)
		{
			Buffer<String> tokens = lines[i].Split(" ");

			if (tokens.GetSize() >= 3)
			{
				if (tokens[0][0] == 'v')
				{
					if (tokens[0].GetLength() == 1) //Vertex position
					{
						if (tokens.GetSize() >= 4)
						{
							positions.Add(Vector3(tokens[1].ToFloat() * scale, tokens[2].ToFloat() * scale, tokens[3].ToFloat() * scale));

							const Vector3 &newPos = positions.Last();

							if (newPos[0] < data.bounds.min[0])
								data.bounds.min[0] = newPos[0];

							if (newPos[1] < data.bounds.min[1])
								data.bounds.min[1] = newPos[1];

							if (newPos[2] < data.bounds.min[2])
								data.bounds.min[2] = newPos[2];

							if (newPos[0] > data.bounds.max[0])
								data.bounds.max[0] = newPos[0];

							if (newPos[1] > data.bounds.max[1])
								data.bounds.max[1] = newPos[1];

							if (newPos[2] > data.bounds.max[2])
								data.bounds.max[2] = newPos[2];
						}
					}

					else if (tokens[0][1] == 'n')	//Vertex normal
					{
						if (tokens.GetSize() >= 4)
							normals.Add(Vector3(tokens[1].ToFloat(), tokens[2].ToFloat(), tokens[3].ToFloat()));
					}

					else if (tokens[0][1] == 't')	//Vertex texture coordinates
						uvs.Add(Vector2(tokens[1].ToFloat(), 1.f - tokens[2].ToFloat()));
				}
				else if (tokens[0][0] == 'f')		//Face
				{
					if (data.vertices.GetSize() == 0)
					{
						size_t maxSizeSoFar = Utilities::Max(Utilities::Max(positions.GetSize(), normals.GetSize()), uvs.GetSize());
						data.vertices.SetSize(maxSizeSoFar);
					}

					if (tokens.GetSize() >= 4)		//Triangle
					{
						uint32 v1 = GetVertexIndex(data.vertices, vertexCount, positions, normals, uvs, tokens[1].GetData());
						uint32 v2 = GetVertexIndex(data.vertices, vertexCount, positions, normals, uvs, tokens[2].GetData());
						uint32 v3 = GetVertexIndex(data.vertices, vertexCount, positions, normals, uvs, tokens[3].GetData());

						size_t last = data.elements.GetSize();
						data.elements.Append(3);

						data.elements[last] = v1;
						data.elements[last + 1] = v3;
						data.elements[last + 2] = v2;
						Vertex17F::CalculateTangents(data.vertices[v1], data.vertices[v3], data.vertices[v2]);

						if (tokens.GetSize() == 5)	//Quad, add another triangle
						{
							uint32 v4 = GetVertexIndex(data.vertices, vertexCount, positions, normals, uvs, tokens[4].GetData());

							size_t last = data.elements.GetSize();
							data.elements.Append(3);

							data.elements[last] = v1;
							data.elements[last + 1] = v4;
							data.elements[last + 2] = v3;
							Vertex17F::CalculateTangents(data.vertices[v1], data.vertices[v4], data.vertices[v3]);
						}
					}
				}
			}
		}
	}

	data.bounds.RecalculateSphereBounds();
	return data;
}
