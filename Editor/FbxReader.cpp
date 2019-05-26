#include "EditorIO.hpp"
#include <Engine/Debug.hpp>

inline Vector3 FbxVec4ToVector3(const FbxVector4 &v)
{
	return Vector3(-v[0], v[2], -v[1]);
}

inline Vector2 FbxVec2ToVector2(const FbxVector2& v)
{
	return Vector2(v[0], v[1]);
}

Vertex17F GetFBXVertex(const FbxMesh* mesh, int polygonIndex, int vertexIndex, const char *uvSet)
{
	Vertex17F v = Vertex17F();
	int control = mesh->GetPolygonVertex(polygonIndex, vertexIndex);
	
	if (control > -1)
		v.pos = FbxVec4ToVector3(mesh->mControlPoints[control]);
	
	FbxVector4 normal;
	if (mesh->GetPolygonVertexNormal(polygonIndex, vertexIndex, normal))
		v.normal = FbxVec4ToVector3(normal);

	FbxVector2 uv;
	bool unmapped;
	if (mesh->GetPolygonVertexUV(polygonIndex, vertexIndex, uvSet, uv, unmapped))
		v.uvOffset = FbxVec2ToVector2(uv);

	return v;
}

uint32 GetVertexIndex(Buffer<Vertex17F>& vertices, uint32& vertexCount, const Vertex17F& vertex)
{
	uint32 i = 0;
	for (; i < vertexCount; ++i)
		if (vertices[i].pos == vertex.pos && vertices[i].normal == vertex.normal && vertices[i].uvOffset == vertex.uvOffset)
			return i;

	if (i == vertices.GetSize())
		vertices.Append(1);

	vertices[i].pos = vertex.pos;
	vertices[i].normal = vertex.normal;
	vertices[i].uvOffset = vertex.uvOffset;

	vertexCount++;
	return i;
}

inline uint32 GetVertexIndex(Buffer<Vertex17F>& vertices, uint32& vertexCount, const FbxMesh *mesh, int polyIndex, int vertIndex, const char *uvSet)
{
	return GetVertexIndex(vertices, vertexCount, GetFBXVertex(mesh, polyIndex, vertIndex, uvSet));
}

ModelData EditorIO::ReadFBXFile(FbxManager *fbxManager, const char* filename)
{
	ModelData data = ModelData();

	FbxImporter* importer = FbxImporter::Create(fbxManager, "");

	if (importer->Initialize(filename, -1, fbxManager->GetIOSettings()))
	{
		FbxScene* scene = FbxScene::Create(fbxManager, "importScene");

		importer->Import(scene);

		importer->Destroy();

		FbxNode* root = scene->GetRootNode();
		if (root)
		{
			for (int i = 0; i < root->GetChildCount(); ++i)
			{
				FbxNode* node = root->GetChild(i);

				FbxMesh* mesh = node->GetMesh();

				if (mesh)
				{
					const char* uvSet = nullptr;

					FbxStringList uvSetNames;
					mesh->GetUVSetNames(uvSetNames);
					if (uvSetNames.GetCount() > 0)
						uvSet = uvSetNames[0];

					uint32 vertexCount = 0;

					int polyCount = mesh->GetPolygonCount();
					for (int i = 0; i < polyCount; ++i)
					{
						int polySize = mesh->GetPolygonSize(i);

						if (polySize <= 2)
						{
							Debug::PrintLine("Warning: FBX polygon with less than 3 vertices?!, ignoring...");
							continue;
						}

						uint32 v1 = GetVertexIndex(data.vertices, vertexCount, mesh, i, 0, uvSet);
						uint32 v2 = GetVertexIndex(data.vertices, vertexCount, mesh, i, 1, uvSet);
						uint32 v3 = GetVertexIndex(data.vertices, vertexCount, mesh, i, 2, uvSet);

						size_t last = data.elements.GetSize();
						data.elements.Append(3);

						data.elements[last] = v1;
						data.elements[last + 1] = v2;
						data.elements[last + 2] = v3;
						Vertex17F::CalculateTangents(data.vertices[v1], data.vertices[v3], data.vertices[v2]);

						if (polySize > 3)	//Quad, add another triangle
						{
							uint32 v4 = GetVertexIndex(data.vertices, vertexCount, mesh, i, 3, uvSet);

							size_t last = data.elements.GetSize();
							data.elements.Append(3);

							data.elements[last] = v1;
							data.elements[last + 1] = v3;
							data.elements[last + 2] = v4;
							Vertex17F::CalculateTangents(data.vertices[v1], data.vertices[v4], data.vertices[v3]);

							if (polySize > 4)
								Debug::Error("Polygons with more than 4 vertices are not supported because I am lazy");
						}
					}
				}
			}
		}

		data.bounds.RecalculateSphereBounds();

		scene->Destroy();
	}

	return data;
}

/*
Buffer<Vector3> vertices;
Buffer<Vector3> normals;
Buffer<Vector2> uvs;

		//CONTROL POINTS (Vertex positions)
		for (int i = 0; i < mesh->mControlPoints.GetCount(); ++i)
		{
			vertices.Add(FbxVec4ToVector3(mesh->mControlPoints[i]));
		}


		//Layer stuff
		for (int l = 0; l < mesh->GetLayerCount(); l++)
		{
			//VERTEX NORMALS
			{
				FbxLayerElementNormal* normalLayer = mesh->GetLayer(l)->GetNormals();

				if (normalLayer)
				{
					auto array = normalLayer->GetDirectArray();

					for (int i = 0; i < array.GetCount(); ++i)
					{
						normals.Add(FbxVec4ToVector3(array[i]));
					}
				}
			}

			//VERTEX UVS
			{
				FbxLayerElementUV* uvLayer = mesh->GetLayer(i)->GetUVs();

				if (uvLayer)
				{
					auto array = uvLayer->GetDirectArray();

					for (int i = 0; i < array.GetCount(); ++i)
					{
						uvs.Add(FbxVec2ToVector2(array[i]));
					}
				}
			}
		}
		*/
