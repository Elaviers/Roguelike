#include "EditorIO.hpp"
#include <Engine/Mesh_Skeletal.hpp>
#include <Engine/Mesh_Static.hpp>

inline Vector3 FbxVector4ToVector3(const FbxVector4& v)
{
	return Vector3((float)v[0], (float)v[1], (float)v[2]);
}

inline Vector2 FbxVector2ToVector2(const FbxVector2& v)
{
	return Vector2((float)v[0], (float)v[1]);
}

inline Mat4 FbxAMatrixToMat4(const FbxAMatrix& m)
{
	Mat4 result;
	for (int r = 0; r < 4; ++r)
		for (int c = 0; c < 4; ++c)
			result[r][c] = (float)m.Get(r, c);

	return result;
}

class FbxMeshDataImporter
{
	struct Vertex
	{
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;

		uint32 cpIndex;
	};

	struct VertexMapping
	{
		uint32 cpIndex;
		float weight;
	};

	Skeleton _skeleton;

	Buffer<Vertex>	_vertices;
	Buffer<uint32>	_elements;

	Hashmap<String, Buffer<VertexMapping>> _vertexGroups;

	Buffer<List<uint32>> _vertsForCP;

	uint32 _totalVertexCount = 0;
	//////

	uint32 _GetIndexForVertex(const Vertex& vertex)
	{
		for (uint32 i = 0; i < _vertices.GetSize(); ++i)
			if (_vertices[i].pos == vertex.pos && _vertices[i].normal == vertex.normal && _vertices[i].uv == vertex.uv)
				return i;

		uint32 index = _vertices.GetSize();
		_vertices.Add(vertex);
		_vertsForCP[vertex.cpIndex].Add(index);
		_totalVertexCount++;
		return index;
	}

	Vertex _ReadMeshVertex(const FbxMesh* mesh, int polyIndex, int vertIndex, const char* uvSet)
	{
		FbxAMatrix fbxTransform = mesh->GetNode()->EvaluateGlobalTransform();
		Mat4 transform = FbxAMatrixToMat4(fbxTransform);
		transform.MultiplyColumn(0, -1.f);

		Vertex result;
		
		int cpIndex = mesh->GetPolygonVertex(polyIndex, vertIndex);
		if (cpIndex < 0)
			Debug::Error("Could not find the control point index of a vertex!");

		result.cpIndex = (uint32)cpIndex;
		result.pos = FbxVector4ToVector3(mesh->mControlPoints[result.cpIndex]) * transform;

		FbxVector4 normal;
		if (mesh->GetPolygonVertexNormal(polyIndex, vertIndex, normal))
			result.normal = FbxVector4ToVector3(normal) * transform;

		FbxVector2 uv;
		bool unmapped;
		if (mesh->GetPolygonVertexUV(polyIndex, vertIndex, uvSet, uv, unmapped))
			result.uv = FbxVector2ToVector2(uv);

		return result;
	}

	void _ReadMeshNode(FbxNode* node)
	{
		FbxMesh* mesh = node->GetMesh();
		if (mesh)
		{
			_totalVertexCount = 0;
			_vertsForCP.SetSize(mesh->mControlPoints.GetCount());

			const char* uvSet = nullptr;

			FbxStringList uvSetNames;
			mesh->GetUVSetNames(uvSetNames);
			if (uvSetNames.GetCount() > 0)
				uvSet = uvSetNames[0];

			int polyCount = mesh->GetPolygonCount();
			for (int i = 0; i < polyCount; ++i)
			{
				int polySize = mesh->GetPolygonSize(i);

				if (polySize <= 2)
				{
					Debug::PrintLine("Warning: FBX polygon with less than 3 vertices?!, ignoring...");
					continue;
				}

				uint32 v1 = _GetIndexForVertex(_ReadMeshVertex(mesh, i, 0, uvSet));
				uint32 v2 = _GetIndexForVertex(_ReadMeshVertex(mesh, i, 1, uvSet));
				uint32 v3 = _GetIndexForVertex(_ReadMeshVertex(mesh, i, 2, uvSet));

				size_t last = _elements.GetSize();
				_elements.Append(3);
				_elements[last] = v1;
				_elements[last + 1] = v2;
				_elements[last + 2] = v3;
				
				if (polySize > 3)	//Quad, add another triangle
				{
					uint32 v4 = _GetIndexForVertex(_ReadMeshVertex(mesh, i, 3, uvSet));

					size_t last = _elements.GetSize();
					_elements.Append(3);
					_elements[last] = v1;
					_elements[last + 1] = v3;
					_elements[last + 2] = v4;
					
					if (polySize > 4)
						Debug::Error("Polygons with more than 4 vertices are not supported because I am lazy");
				}
			}

			for (int i = 0; i < mesh->GetDeformerCount(); ++i)
			{
				FbxDeformer* deformer = mesh->GetDeformer(i);
				auto deformerType = deformer->GetDeformerType();

				if (deformerType == FbxDeformer::eSkin)
				{
					FbxSkin* skin = FbxCast<FbxSkin>(deformer);
					if (skin)
					{
						auto skinningType = skin->GetSkinningType();

						for (int j = 0; j < skin->GetClusterCount(); ++j)
						{
							auto cluster = skin->GetCluster(j);
							int* cpIndices = cluster->GetControlPointIndices();
							double* cpWeights = cluster->GetControlPointWeights();
							int cpIndexCount = cluster->GetControlPointIndicesCount();

							Buffer<VertexMapping>& dest = _vertexGroups[cluster->GetName()];
							dest.SetSize(cpIndexCount);
							for (int k = 0; k < cpIndexCount; ++k)
								dest[k] = VertexMapping{ (uint32)cpIndices[k], (float)cpWeights[k] };

							Joint* joint = _skeleton.GetJointWithName(cluster->GetName());
							if (joint)
							{
								FbxAMatrix fbxBinding = cluster->GetLink()->EvaluateGlobalTransform().Inverse();
								joint->bindingMatrix = FbxAMatrixToMat4(fbxBinding);
								joint->bindingMatrix.MultiplyColumn(0, -1.f);
							}
							else Debug::Error(CSTR("Warning: FBX deformer \"", cluster->GetName(), "\" does not have a corresponding joint!"));
						}
					}
				}
			}
		}
	}

	void _ReadSkeletonNode(FbxNode* node, Joint* parent = nullptr)
	{
		FbxSkeleton* skeleton = node->GetSkeleton();
		if (skeleton)
		{
			Joint* joint = _skeleton.CreateJoint(parent);

			if (joint == nullptr)
			{
				Debug::Error("Bone error");
				return;
			}

			joint->name = node->GetSkeleton()->GetName();
			FbxTransform& fbxt = node->GetTransform();

			for (int i = 0; i < node->GetChildCount(); ++i)
				_ReadSkeletonNode(node->GetChild(i), joint);
		}
	}

	void _ReadSkeletonNodesRecursive(FbxNode* root)
	{
		if (root->GetSkeleton())
		{
			_ReadSkeletonNode(root);
			return;
		}

		for (int i = 0; i < root->GetChildCount(); ++i)
			_ReadSkeletonNodesRecursive(root->GetChild(i));
	}

	void _ReadMeshNodesRecursive(FbxNode* root)
	{
		if (root->GetMesh())
			_ReadMeshNode(root);

		for (int i = 0; i < root->GetChildCount(); ++i)
			_ReadMeshNodesRecursive(root->GetChild(i));
	}

public:
	Mesh* Import(FbxManager* fbxManager, const char* filename)
	{
		FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");

		if (fbxImporter->Initialize(filename, -1, fbxManager->GetIOSettings()))
		{
			FbxScene* scene = FbxScene::Create(fbxManager, "import");
			
			FbxAxisSystem::OpenGL.ConvertScene(scene);

			fbxImporter->Import(scene);
			fbxImporter->Destroy();
			
			FbxNode* root = scene->GetRootNode();
			if (root)
			{
				_ReadSkeletonNodesRecursive(root);
				_ReadMeshNodesRecursive(root);

				if (_totalVertexCount == 0)
					return nullptr;
				
				if (_skeleton.GetJointCount() > 0)
				{
					Mesh_Skeletal* mesh = new Mesh_Skeletal();
					mesh->skeleton = _skeleton;
					mesh->vertices.SetSize(_vertices.GetSize());
					mesh->elements.SetSize(_elements.GetSize());

					Vector3 boundsMin(1000, 1000, 1000), boundsMax(-1000, -1000, -1000);

					//Copy vertex info
					for (uint32 i = 0; i < _totalVertexCount; ++i)
					{
						const Vertex& src = _vertices[i];
						VertexSkeletal& dest = mesh->vertices[i];
						dest.pos = src.pos;
						dest.normal = src.normal;
						dest.uv = src.uv;

						boundsMin = Vector3(Utilities::Min(boundsMin[0], src.pos[0]), Utilities::Min(boundsMin[1], src.pos[1]), Utilities::Min(boundsMin[2], src.pos[2]));
						boundsMax = Vector3(Utilities::Max(boundsMax[0], src.pos[0]), Utilities::Max(boundsMax[1], src.pos[1]), Utilities::Max(boundsMax[2], src.pos[2]));
					}

					//Bounds
					mesh->bounds.min = boundsMin;
					mesh->bounds.max = boundsMax;
					mesh->bounds.RecalculateSphereBounds();

					//Elements & TBN
					for (size_t i = 0; i < _elements.GetSize(); ++i)
					{
						mesh->elements[i] = _elements[i];
					
						if ((i - 2) % 3 == 0)
							VertexSkeletal::CalculateTangents(mesh->vertices[0], mesh->vertices[1], mesh->vertices[2]);
					}

					//Vertex bone info
					for (size_t jointIndex = 0; jointIndex < _skeleton.GetJointCount(); ++jointIndex)
					{
						Joint* joint = _skeleton.GetJointWithID((int)jointIndex);
						Buffer<VertexMapping>* vertGroups = _vertexGroups.Get(joint->name);
						if (vertGroups)
						{
							for (size_t vertGroupIndex = 0; vertGroupIndex < vertGroups->GetSize(); ++vertGroupIndex)
							{
								VertexMapping& vertexMapping = (*vertGroups)[vertGroupIndex];

								List<uint32>& affectedVerts = _vertsForCP[vertexMapping.cpIndex];
								for (auto iVertexIndex = affectedVerts.First(); iVertexIndex; ++iVertexIndex)
								{
									VertexSkeletal& vertex = mesh->vertices[*iVertexIndex];

									for (int slot = 0; slot < VertexSkeletal::BONE_COUNT; ++slot)
										if (vertex.boneWeights[slot] == 0.f)
										{
											vertex.boneIndices[slot] = *iVertexIndex;
											vertex.boneWeights[slot] = vertexMapping.weight;
										}
								}
							}
						}
					}

					return mesh;
				}

				//Load as static mesh
				{
					Mesh_Static* mesh = new Mesh_Static();
					mesh->vertices.SetSize(_vertices.GetSize());
					mesh->elements.SetSize(_elements.GetSize());

					Vector3 boundsMin, boundsMax;

					//Copy vertex info
					for (uint32 i = 0; i < _totalVertexCount; ++i)
					{
						const Vertex& src = _vertices[i];
						Vertex17F& dest = mesh->vertices[i];
						dest.pos = src.pos;
						dest.normal = src.normal;
						dest.uv = src.uv;

						boundsMin = Vector3(Utilities::Min(boundsMin[0], src.pos[0]), Utilities::Min(boundsMin[1], src.pos[1]), Utilities::Min(boundsMin[2], src.pos[2]));
						boundsMax = Vector3(Utilities::Max(boundsMax[0], src.pos[0]), Utilities::Max(boundsMax[1], src.pos[1]), Utilities::Max(boundsMax[2], src.pos[2]));
					}

					//Bounds
					mesh->bounds.min = boundsMin;
					mesh->bounds.max = boundsMax;
					mesh->bounds.RecalculateSphereBounds();

					//Elements & TBN
					for (size_t i = 0; i < _elements.GetSize(); ++i)
					{
						mesh->elements[i] = _elements[i];

						if ((i - 2) % 3 == 0)
							Vertex17F::CalculateTangents(mesh->vertices[0], mesh->vertices[1], mesh->vertices[2]);
					}

					return mesh;
				}
			}
		}

		fbxImporter->Destroy();
		return nullptr;
	}
};

Mesh* EditorIO::ReadFBXMesh(FbxManager* fbxManager, const char* filename)
{
	return FbxMeshDataImporter().Import(fbxManager, filename);
}

Animation* EditorIO::ReadFBXAnimation(FbxManager* fbxManager, const char* filename)
{
	return nullptr;
}
