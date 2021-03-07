#include "EditorIO.hpp"
#include <ELGraphics/Mesh_Skeletal.hpp>
#include <ELGraphics/Mesh_Static.hpp>

inline Vector3 FbxVector4ToVector3(const FbxVector4& v)
{
	return Vector3((float)v[0], (float)v[1], (float)v[2]);
}

inline Vector2 FbxVector2ToVector2(const FbxVector2& v)
{
	return Vector2((float)v[0], (float)v[1]);
}

inline Matrix4 FbxAMatrixToMatrix4(const FbxAMatrix& m)
{
	Matrix4 result;
	for (int r = 0; r < 4; ++r)
		for (int c = 0; c < 4; ++c)
			result[r][c] = (float)m.Get(r, c);

	return result;
}

inline FbxAMatrix Matrix4ToFbxAMatrix(const Matrix4& m)
{
	FbxAMatrix result;
	for (int r = 0; r < 4; ++r)
		for (int c = 0; c < 4; ++c)
			result.mData[r][c] = m[r][c];

	return result;
}

inline void FbxSpaceToEngineSpace(FbxAMatrix& m)
{
	m.mData[3][2] *= -1.f;
}

inline void EngineSpaceToFbxSpace(FbxAMatrix& m)
{
	m.mData[3][2] *= -1.f;
}

class FbxMeshDataImporter
{
	struct Vertex
	{
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;

		uint32 cpIndex = 0;
	};

	struct VertexMapping
	{
		uint32 cpIndex = 0;
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

		uint32 index = (uint32)_vertices.GetSize();
		_vertices.Add(vertex);
		_vertsForCP[vertex.cpIndex].Add(index);
		_totalVertexCount++;
		return index;
	}

	Vertex _ReadMeshVertex(const FbxMesh* mesh, int polyIndex, int vertIndex, const char* uvSet)
	{
		FbxAMatrix fbxTransform = mesh->GetNode()->EvaluateGlobalTransform();

		//Flip Z on import
		fbxTransform.mData[0][2] *= -1.f;
		fbxTransform.mData[1][2] *= -1.f;
		fbxTransform.mData[2][2] *= -1.f;
		fbxTransform.mData[3][2] *= -1.f;

		Matrix4 transform = FbxAMatrixToMatrix4(fbxTransform);

		Vertex result;
		
		int cpIndex = mesh->GetPolygonVertex(polyIndex, vertIndex);
		if (cpIndex < 0)
			Debug::Error("Could not find the control point index of a vertex!");

		result.cpIndex = (uint32)cpIndex;
		result.pos = (Vector4(FbxVector4ToVector3(mesh->mControlPoints[result.cpIndex]), 1.f) * transform).GetXYZ();

		FbxVector4 fbxNormal;
		if (mesh->GetPolygonVertexNormal(polyIndex, vertIndex, fbxNormal))
		{
			Vector4 normal = Vector4(FbxVector4ToVector3(fbxNormal), 0.f) * transform;
			result.normal = Vector3(normal[0], normal[1], normal[2]).Normalise();
		}

		FbxVector2 uv;
		bool unmapped;
		if (mesh->GetPolygonVertexUV(polyIndex, vertIndex, uvSet, uv, unmapped))
		{
			result.uv = FbxVector2ToVector2(uv);
			result.uv.y = 1.f - result.uv.y;
		}

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
				_elements.Grow(3);
				_elements[last] = v1;
				_elements[last + 1] = v2;
				_elements[last + 2] = v3;
				
				if (polySize > 3)	//Quad, add another triangle
				{
					uint32 v4 = _GetIndexForVertex(_ReadMeshVertex(mesh, i, 3, uvSet));

					size_t last = _elements.GetSize();
					_elements.Grow(3);
					_elements[last] = v1;
					_elements[last + 1] = v3;
					_elements[last + 2] = v4;
					
					if (polySize > 4)
						Debug::Error("Polygons with more than 4 vertices are not supported because I am lazy");
				}
			}

			FbxAMatrix nodeTransform = node->EvaluateGlobalTransform();

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
								FbxAMatrix binding = cluster->GetLink()->EvaluateGlobalTransform().Inverse();
								FbxSpaceToEngineSpace(binding);
								joint->bindingMatrix = FbxAMatrixToMatrix4(binding);

								FbxAMatrix lt = cluster->GetLink()->EvaluateLocalTransform();
								FbxSpaceToEngineSpace(lt);

								FbxAMatrix gt = cluster->GetLink()->EvaluateGlobalTransform();
								
								EngineSpaceToFbxSpace(binding);
								FbxAMatrix startSkinning = gt * binding;
								FbxSpaceToEngineSpace(startSkinning);

								FbxSpaceToEngineSpace(gt);

								FbxAMatrix trueStartSkinning = cluster->GetLink()->EvaluateGlobalTransform() * cluster->GetLink()->EvaluateGlobalTransform().Inverse();
								FbxSpaceToEngineSpace(trueStartSkinning);

								Debug::PrintLine(cluster->GetName());
								Debug::PrintLine("{");
								Debug::PrintLine(CSTR("\tNodeTranslation: ", FbxVector4ToVector3(nodeTransform.GetT())));
								Debug::PrintLine(CSTR("\tNodeRotation: ", FbxVector4ToVector3(nodeTransform.GetR())));
								Debug::PrintLine(CSTR("\tNodeScaling: ", FbxVector4ToVector3(nodeTransform.GetS())));
								Debug::PrintLine(CSTR("\n\tLocalLinkTranslation: ", FbxVector4ToVector3(lt.GetT())));
								Debug::PrintLine(CSTR("\tLocalLinkRotation: ", FbxVector4ToVector3(lt.GetR())));
								Debug::PrintLine(CSTR("\tLocalLinkScaling: ", FbxVector4ToVector3(lt.GetS())));
								Debug::PrintLine(CSTR("\n\tGlobalLinkTranslation: ", FbxVector4ToVector3(gt.GetT())));
								Debug::PrintLine(CSTR("\tGlobalLinkRotation: ", FbxVector4ToVector3(gt.GetR())));
								Debug::PrintLine(CSTR("\tGlobalLinkScaling: ", FbxVector4ToVector3(gt.GetS())));
								Debug::PrintLine(CSTR("\n\tBindingTranslation: ", FbxVector4ToVector3(binding.GetT())));
								Debug::PrintLine(CSTR("\tBindingRotation: ", FbxVector4ToVector3(binding.GetR())));
								Debug::PrintLine(CSTR("\tBindingScaling: ", FbxVector4ToVector3(binding.GetS())));
								Debug::PrintLine(CSTR("\n\tInitialSkinningTranslation: ", FbxVector4ToVector3(startSkinning.GetT())));
								Debug::PrintLine(CSTR("\tInitialSkinningRotation: ", FbxVector4ToVector3(startSkinning.GetR())));
								Debug::PrintLine(CSTR("\tInitialSkinningScaling: ", FbxVector4ToVector3(startSkinning.GetS())));
								Debug::PrintLine(CSTR("\n\tTrueInitialSkinningTranslation: ", FbxVector4ToVector3(trueStartSkinning.GetT())));
								Debug::PrintLine(CSTR("\tTrueInitialSkinningRotation: ", FbxVector4ToVector3(trueStartSkinning.GetR())));
								Debug::PrintLine(CSTR("\tTrueInitialSkinningScaling: ", FbxVector4ToVector3(trueStartSkinning.GetS())));
								Debug::PrintLine("}");
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
			
			fbxImporter->Import(scene);
			fbxImporter->Destroy();
			FbxAxisSystem::OpenGL.ConvertScene(scene);

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

						boundsMin = Vector3(Maths::Min(boundsMin.x, src.pos.x), Maths::Min(boundsMin.y, src.pos.y), Maths::Min(boundsMin.z, src.pos.z));
						boundsMax = Vector3(Maths::Max(boundsMax.x, src.pos.x), Maths::Max(boundsMax.y, src.pos.y), Maths::Max(boundsMax.z, src.pos.z));
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
							for (size_t vgIndex = 0; vgIndex < vertGroups->GetSize(); ++vgIndex)
							{
								VertexMapping& vertexMapping = (*vertGroups)[vgIndex];

								List<uint32>& affectedVerts = _vertsForCP[vertexMapping.cpIndex];
								for (uint32 iVertexIndex : affectedVerts)
								{
									VertexSkeletal& vertex = mesh->vertices[iVertexIndex];

									for (int slot = 0; slot < VertexSkeletal::BONE_COUNT; ++slot)
										if (vertex.boneWeights[slot] == 0.f)
										{
											vertex.boneIndices[slot] = (uint32)jointIndex;
											vertex.boneWeights[slot] = vertexMapping.weight;
											break;
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

						boundsMin = Vector3(Maths::Min(boundsMin.x, src.pos.x), Maths::Min(boundsMin.y, src.pos.y), Maths::Min(boundsMin.z, src.pos.z));
						boundsMax = Vector3(Maths::Max(boundsMax.x, src.pos.x), Maths::Max(boundsMax.y, src.pos.y), Maths::Max(boundsMax.z, src.pos.z));
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

class FbxAnimationImporter
{
	const Skeleton* _skeleton;
	Animation* _result;

	float _startTime;
	float _endTime;
	float _frameLength;

public:
	FbxAnimationImporter(const Skeleton& skeleton) : _skeleton(&skeleton), _result(nullptr), _startTime(0.f), _endTime(0.f), _frameLength(0.f) {}

	void _ReadMesh(FbxNode* node)
	{
		FbxMesh* mesh = node->GetMesh();
		if (mesh)
		{
			for (int i = 0; i < mesh->GetDeformerCount(); ++i)
			{
				FbxDeformer* deformer = mesh->GetDeformer(i);
				auto deformerType = deformer->GetDeformerType();

				if (deformerType == FbxDeformer::eSkin)
				{
					FbxSkin* skin = FbxCast<FbxSkin>(deformer);
					if (skin)
					{
						FbxTime fbxTime;

						for (float t = _startTime; t <= _endTime; t += _frameLength)
						{
							fbxTime.SetSecondDouble(t);

							for (int j = 0; j < skin->GetClusterCount(); ++j)
							{
								auto cluster = skin->GetCluster(j);
								const char* name = cluster->GetName();
								const Joint* joint = _skeleton->GetJointWithName(name);

								if (joint)
								{
									FbxAMatrix binding = Matrix4ToFbxAMatrix(joint->bindingMatrix);
									EngineSpaceToFbxSpace(binding);

									FbxAMatrix transform = cluster->GetLink()->EvaluateGlobalTransform(fbxTime) * binding;
									FbxSpaceToEngineSpace(transform);

									Vector3 fbxRotation = FbxVector4ToVector3(transform.GetR());
									fbxRotation.y *= -1.f;
									fbxRotation.z *= -1.f;

									_result->GetTranslationTrack(name).	AddKey(t, FbxVector4ToVector3(transform.GetT()));
									_result->GetRotationTrack(name).	AddKey(t, Quaternion::FromEulerZYX(fbxRotation));
									_result->GetScalingTrack(name).		AddKey(t, FbxVector4ToVector3(transform.GetS()));
								}
								else
									Debug::Error(CSTR("Animation track \"", name, "\" has no corresponding joint!"));
							}
						}
					}
				}
			}
		}
	}

	void _ReadNodesRecursive(FbxNode* root)
	{
		if (root->GetMesh())
			_ReadMesh(root);

		for (int i = 0; i < root->GetChildCount(); ++i)
			_ReadNodesRecursive(root->GetChild(i));
	}

	Animation* Import(FbxManager* fbxManager, const char* filename)
	{
		FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");

		if (fbxImporter->Initialize(filename, -1, fbxManager->GetIOSettings()))
		{
			FbxScene* scene = FbxScene::Create(fbxManager, "import");

			fbxImporter->Import(scene);
			fbxImporter->Destroy();
			FbxAxisSystem::OpenGL.ConvertScene(scene);

			FbxNode* root = scene->GetRootNode();
			if (root)
			{
				_startTime = (float)scene->GetCurrentAnimationStack()->GetLocalTimeSpan().GetStart().GetSecondDouble();
				_endTime = (float)scene->GetCurrentAnimationStack()->GetLocalTimeSpan().GetStop().GetSecondDouble();
				_frameLength = 1.f / 60.f;

				_result = new Animation();
				_ReadNodesRecursive(root);
				return _result;
			}
		}

		fbxImporter->Destroy();
		return nullptr;
	}
};

Animation* EditorIO::ReadFBXAnimation(FbxManager* fbxManager, const char* filename, const Skeleton& skeleton)
{
	return FbxAnimationImporter(skeleton).Import(fbxManager, filename);
}
