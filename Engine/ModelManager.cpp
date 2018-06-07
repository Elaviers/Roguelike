#include "ModelManager.h"
#include "Error.h"
#include "IO.h"
#include "Utilities.h"
#include "Vector.h"

ModelManager::ModelManager()
{
}

ModelManager::~ModelManager()
{
	_models.ForEach(
		[](Model &model) 
		{
			model.Delete(); 
		});
}

void ModelManager::Initialise()
{
	Vertex17F cubeData[36] =
	{
		//Bottom
		VERT14F_TRI(Vector3(1, -1, -1),		Vector3(-1, -1, -1),	Vector3(1, -1, 1),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(0, -1, 0)),
		VERT14F_TRI(Vector3(-1, -1, 1),		Vector3(1, -1, 1),		Vector3(-1, -1, -1),Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(0, -1, 0)),

		//Top
		VERT14F_TRI(Vector3(-1, 1, -1),		Vector3(1, 1, -1),		Vector3(-1, 1, 1),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(0, 1, 0)),
		VERT14F_TRI(Vector3(1, 1, 1),		Vector3(-1, 1, 1),		Vector3(1, 1, -1),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(0, 1, 0)),

		//Front
		VERT14F_TRI(Vector3(-1, -1, -1),	Vector3(1, -1, -1),		Vector3(-1, 1, -1),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(0, 0, -1)),
		VERT14F_TRI(Vector3(1, 1, -1),		Vector3(-1, 1, -1),		Vector3(1, -1, -1),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(0, 0, -1)),

		//Right
		VERT14F_TRI(Vector3(1, -1, -1),		Vector3(1, -1, 1),		Vector3(1, 1, -1),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(1, 0, 0)),
		VERT14F_TRI(Vector3(1, 1, 1),		Vector3(1, 1, -1),		Vector3(1, -1, 1),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(1, 0, 0)),

		//Back
		VERT14F_TRI(Vector3(1, -1, 1),		Vector3(-1, -1, 1),		Vector3(1, 1, 1),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(0, 0, 1)),
		VERT14F_TRI(Vector3(-1, 1, 1),		Vector3(1, 1, 1),		Vector3(-1, -1, 1),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(0, 0, 1)),

		//Left
		VERT14F_TRI(Vector3(-1, -1, 1),		Vector3(-1, -1, -1),	Vector3(-1, 1, 1),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(-1, 0, 0)),
		VERT14F_TRI(Vector3(-1, 1, -1),		Vector3(-1, 1, 1),		Vector3(-1, -1, -1),Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(-1, 0, 0)),
	};

	for (int i = 0; i < 36; i += 3)
		Vertex17F::CalculateTangents(cubeData[i], cubeData[i + 1], cubeData[i + 2]);

	_cube.Create(cubeData, 36);

	for (int i = 0; i < 36; i += 3) {
		cubeData[i].normal *= -1;
		cubeData[i + 1].normal *= -1;
		cubeData[i + 2].normal *= -1;

		Utilities::Swap(cubeData[i + 1], cubeData[i + 2]);
		Vertex17F::CalculateTangents(cubeData[i], cubeData[i + 1], cubeData[i + 2]);
	}

	_invCube.Create(cubeData, 36);

	const Vector3 planeColour(1.f, 1.f, 1.f);

	Vertex17F planeVerts[4] =
	{
		{ Vector3(-1, 0, -1), Vector2(0, 0), planeColour, Vector3(), Vector3(), Vector3(0, 1, 0) },
		{ Vector3(1, 0, -1), Vector2(1, 0), planeColour, Vector3(), Vector3(), Vector3(0, 1, 0) },
		{ Vector3(-1, 0, 1), Vector2(0, 1), planeColour, Vector3(), Vector3(), Vector3(0, 1, 0) },
		{ Vector3(1, 0, 1), Vector2(1, 1), planeColour, Vector3(), Vector3(), Vector3(0, 1, 0) }
	};

	uint32 planeElements[6] =
	{
		0, 1, 2,
		3, 2, 1
	};

	Vertex17F::CalculateTangents(planeVerts[0], planeVerts[1], planeVerts[2]);
	Vertex17F::CalculateTangents(planeVerts[1], planeVerts[2], planeVerts[3]); //Not good

	_plane.Create(planeVerts, 4, planeElements, 6);
}

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
inline const uint32 OBJIndexToCIndex(const Buffer<T> &buf, int32 objIndex)
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
	const Vector2 &uv = uvs[OBJIndexToCIndex(uvs, objVertex.uv_index)];

	uint32 i = 0;
	for (; i < vertexCount; ++i)
		if (vertices[i].pos == pos && vertices[i].normal == normal && vertices[i].uv == uv)
			return i;

	if (i == vertices.GetSize())
		vertices.Append(1);

	vertices[i].pos = pos;
	vertices[i].normal = normal;
	vertices[i].uv = uv;

	vertexCount++;
	return i;
}

void ModelManager::LoadModel(const char *filename, const char *name)
{
	const float scale = 1;

	String objSrc = IO::ReadFileString(filename);
	if (objSrc.GetLength())
	{
		Buffer<Vertex17F> vertices;
		Buffer<uint32> elements;

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
							positions.Add(Vector3(tokens[1].ToFloat() * scale, tokens[2].ToFloat() * scale, tokens[3].ToFloat() * scale));
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
					if (vertices.GetSize() == 0)
					{
						uint32 maxSizeSoFar = Utilities::Max(Utilities::Max(positions.GetSize(), normals.GetSize()), uvs.GetSize());
						vertices.SetSize(maxSizeSoFar);
					}

					if (tokens.GetSize() >= 4)		//Triangle
					{
						uint32 v1 = GetVertexIndex(vertices, vertexCount, positions, normals, uvs, tokens[1].GetData());
						uint32 v2 = GetVertexIndex(vertices, vertexCount, positions, normals, uvs, tokens[2].GetData());
						uint32 v3 = GetVertexIndex(vertices, vertexCount, positions, normals, uvs, tokens[3].GetData());

						uint32 last = elements.GetSize();
						elements.Append(3);

						elements[last] =		v1;
						elements[last + 1] =	v3;
						elements[last + 2] =	v2;
						Vertex17F::CalculateTangents(vertices[v1], vertices[v3], vertices[v2]);

						if (tokens.GetSize() == 5)	//Quad, add another triangle
						{
							uint32 v4 = GetVertexIndex(vertices, vertexCount, positions, normals, uvs, tokens[4].GetData());

							uint32 last = elements.GetSize();
							elements.Append(3);

							elements[last] =		v1;
							elements[last + 1] =	v4;
							elements[last + 2] =	v3;
							Vertex17F::CalculateTangents(vertices[v1], vertices[v4], vertices[v3]);
						}
					}
				}
			}
		}

		Model *newModel = _models.New(String(name));

		if (newModel)
			newModel->Create(vertices.Data(), vertices.GetSize(), elements.Data(), elements.GetSize());
	}
	else
		Error(CSTR("OBJ reader could not open \"" + filename + '\"'));
}
