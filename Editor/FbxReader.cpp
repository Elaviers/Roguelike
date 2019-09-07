#include "EditorIO.hpp"
#include <Engine/Animation.hpp>
#include <Engine/Debug.hpp>
#include <Engine/Hashmap.hpp>
#include <Engine/Mesh_Skeletal.hpp>
#include <Engine/Mesh_Static.hpp>

inline Vector3 FbxVec4ToVector3(const FbxVector4& v)
{
	return Vector3((float)-v[0], (float)v[2], (float)-v[1]);
}

inline Vector3 FbxD3ToVector3(const FbxDouble3& v)
{
	return Vector3((float)-v[0], (float)v[2], (float)-v[1]);
}

inline Vector2 FbxVec2ToVector2(const FbxVector2& v)
{
	return Vector2((float)v[0], (float)v[1]);
}

//Returns control index
int GetFBXVertex(Vertex17F& out, const FbxMesh* fbxMesh, int polygonIndex, int vertexIndex, const char* uvSet)
{
	Vertex17F v = Vertex17F();
	int control = fbxMesh->GetPolygonVertex(polygonIndex, vertexIndex);

	if (control > -1)
		out.pos = FbxVec4ToVector3(fbxMesh->mControlPoints[control]);

	FbxVector4 normal;
	if (fbxMesh->GetPolygonVertexNormal(polygonIndex, vertexIndex, normal))
		out.normal = FbxVec4ToVector3(normal);

	FbxVector2 uv;
	bool unmapped;
	if (fbxMesh->GetPolygonVertexUV(polygonIndex, vertexIndex, uvSet, uv, unmapped))
		out.uvOffset = FbxVec2ToVector2(uv);

	return control;
}

class FbxMeshDataImporter
{
	Skeleton _skeleton;

	Buffer<Vertex17F> _vertices;
	Buffer<uint32> _elements;
	Hashmap<String, Buffer<uint32>> _vertexMappings;

	Buffer<List<uint32>> _vertsForCP;	//List of vertex indices for each control point

	uint32 _totalVertexCount = 0;
	//


	uint32 GetVertexIndex(const Vertex17F& vertex, int cpIndex)
	{
		uint32 i = 0;
		for (; i < _vertices.GetSize(); ++i)
			if (_vertices[i].pos == vertex.pos && _vertices[i].normal == vertex.normal && _vertices[i].uvOffset == vertex.uvOffset)
				return i;

		if (i == _vertices.GetSize())
		{
			_vertices.Append(1);
			_vertsForCP[cpIndex].Add((uint32)(_vertices.GetSize() - 1));
		}

		_vertices[i].pos = vertex.pos;
		_vertices[i].normal = vertex.normal;
		_vertices[i].uvOffset = vertex.uvOffset;

		_totalVertexCount++;
		return i;
	}

	inline uint32 GetVertexIndex(const FbxMesh* fbxMesh, int polyIndex, int vertIndex, const char* uvSet)
	{
		Vertex17F vertex = Vertex17F();
		int cpIndex = GetFBXVertex(vertex, fbxMesh, polyIndex, vertIndex, uvSet);
		return GetVertexIndex(vertex, cpIndex);
	}

	void _LoadMesh(FbxMesh* fbxMesh)
	{
		_totalVertexCount = 0;
		_vertsForCP.SetSize(fbxMesh->mControlPoints.GetCount());

		const char* uvSet = nullptr;

		FbxStringList uvSetNames;
		fbxMesh->GetUVSetNames(uvSetNames);
		if (uvSetNames.GetCount() > 0)
			uvSet = uvSetNames[0];

		int polyCount = fbxMesh->GetPolygonCount();
		for (int i = 0; i < polyCount; ++i)
		{
			int polySize = fbxMesh->GetPolygonSize(i);

			if (polySize <= 2)
			{
				Debug::PrintLine("Warning: FBX polygon with less than 3 vertices?!, ignoring...");
				continue;
			}

			uint32 v1 = GetVertexIndex(fbxMesh, i, 0, uvSet);
			uint32 v2 = GetVertexIndex(fbxMesh, i, 1, uvSet);
			uint32 v3 = GetVertexIndex(fbxMesh, i, 2, uvSet);

			size_t last = _elements.GetSize();
			_elements.Append(3);

			_elements[last] = v1;
			_elements[last + 1] = v2;
			_elements[last + 2] = v3;
			Vertex17F::CalculateTangents(_vertices[v1], _vertices[v3], _vertices[v2]);

			if (polySize > 3)	//Quad, add another triangle
			{
				uint32 v4 = GetVertexIndex(fbxMesh, i, 3, uvSet);

				size_t last = _elements.GetSize();
				_elements.Append(3);

				_elements[last] = v1;
				_elements[last + 1] = v3;
				_elements[last + 2] = v4;
				Vertex17F::CalculateTangents(_vertices[v1], _vertices[v4], _vertices[v3]);

				if (polySize > 4)
					Debug::Error("Polygons with more than 4 vertices are not supported because I am lazy");
			}
		}

		for (int i = 0; i < fbxMesh->GetDeformerCount(); ++i)
		{
			FbxDeformer* d = fbxMesh->GetDeformer(i);
			auto type = d->GetDeformerType();

			if (type == FbxDeformer::eSkin)
			{
				FbxSkin* skin = FbxCast<FbxSkin>(d);

				if (skin)
				{
					auto skinningType = skin->GetSkinningType();

					for (int j = 0; j < skin->GetClusterCount(); ++j)
					{
						auto cluster = skin->GetCluster(j);

						int* cpIndices = cluster->GetControlPointIndices();
						int cpIndexCount = cluster->GetControlPointIndicesCount();

						Buffer<uint32>& dest = _vertexMappings[cluster->GetName()];
						dest.SetSize(cpIndexCount);
						for (int k = 0; k < cpIndexCount; ++k)
							dest[k] = cpIndices[k];
					}
				}
				else Debug::Error("poop error");
			}
		}
	}

	void _LoadSkeleton(FbxNode* node, Joint* parentJoint = nullptr)
	{
		Joint* joint = _skeleton.CreateJoint(parentJoint);

		if (joint == nullptr)
		{
			Debug::Error("Bone error");
			return;
		}

		joint->name = node->GetSkeleton()->GetName();
		FbxTransform& fbxt = node->GetTransform();

		for (int i = 0; i < node->GetChildCount(); ++i)
			_LoadSkeleton(node->GetChild(i), joint);

		if (parentJoint == nullptr)
		{
			FbxScene* scene = node->GetScene();

			int animStackCount = scene->GetSrcObjectCount<FbxAnimStack>();

			for (int i = 0; i < animStackCount; ++i)
			{
				FbxAnimStack* animStack = scene->GetSrcObject<FbxAnimStack>(i);

				int animLayerCount = animStack->GetMemberCount<FbxAnimLayer>();

				for (int j = 0; j < animLayerCount; ++j)
				{
					FbxAnimLayer* layer = animStack->GetMember<FbxAnimLayer>(i);


				}
			}
		}
	}

	void _LoadNodeChildren(FbxNode* parent, FbxNode*& skeletonNode, FbxTransform* parentTransform = nullptr)
	{
		for (int i = 0; i < parent->GetChildCount(); ++i)
		{
			FbxNode* node = parent->GetChild(i);

			FbxMesh* fbxMesh = node->GetMesh();

			if (fbxMesh)
				_LoadMesh(fbxMesh);

			if (node->GetSkeleton())
			{
				_skeleton.Clear();

				_LoadSkeleton(node);
				skeletonNode = node;
				continue;
			}

			_LoadNodeChildren(node, skeletonNode);
		}
	}

public:
	Mesh* Import(FbxManager *fbxManager, FbxImporter *fbxImporter)
	{
		FbxScene* scene = FbxScene::Create(fbxManager, "importScene");

		fbxImporter->Import(scene);

		fbxImporter->Destroy();

		FbxNode* skeletonNode = nullptr;
		FbxNode* root = scene->GetRootNode();

		if (root)
		{
			_LoadNodeChildren(root, skeletonNode);
		}

		scene->Destroy();

		if (_vertices.GetSize())
		{
			if (!_vertexMappings.IsEmpty() && _skeleton.GetJointCount())
			{
				Mesh_Skeletal* skeletalMesh = new Mesh_Skeletal();

				Buffer<uint32> nextBoneIndices;	//Next bone index for each vertex
				skeletalMesh->vertices.SetSize(_vertices.GetSize());
				nextBoneIndices.SetSize(_vertices.GetSize());

				for (size_t i = 0; i < skeletalMesh->vertices.GetSize(); ++i)
				{
					nextBoneIndices[i] = 0;
					skeletalMesh->vertices[i].pos = _vertices[i].pos;
					skeletalMesh->vertices[i].normal = _vertices[i].normal;
					skeletalMesh->vertices[i].tangent = _vertices[i].tangent;
					skeletalMesh->vertices[i].bitangent = _vertices[i].bitangent;
					skeletalMesh->vertices[i].uvOffset = _vertices[i].uvOffset;
				}

				for (auto it = _skeleton.FirstListElement(); it; ++it)
				{
					Buffer<uint32>* affectedCPs = _vertexMappings.Get(it->name);

					if (affectedCPs)
					{
						int jointID = it->GetID();

						for (size_t i = 0; i < affectedCPs->GetSize(); ++i)
						{
							List<uint32>& affectedVerts = _vertsForCP[(*affectedCPs)[i]];

							for (auto it = affectedVerts.First(); it; ++it)
							{
								uint32 vertIndex = *it;
								uint32 boneIndex = nextBoneIndices[vertIndex]++;

								if (boneIndex < 2)
								{
									skeletalMesh->vertices[vertIndex].boneIndices[boneIndex] = jointID;
									skeletalMesh->vertices[vertIndex].boneWeights[boneIndex] = 1.f;
								}
							}
						}
					}
				}

				skeletalMesh->elements = std::move(_elements);
				skeletalMesh->skeleton = std::move(_skeleton);

				skeletalMesh->bounds.RecalculateSphereBounds();

				return skeletalMesh;
			}

			{
				Mesh_Static* staticMesh = new Mesh_Static();

				staticMesh->vertices = std::move(_vertices);

				staticMesh->elements = std::move(_elements);
				staticMesh->bounds.RecalculateSphereBounds();

				return staticMesh;
			}
		}

		return nullptr;
	}
};

Mesh* EditorIO::ReadFBXMesh(FbxManager *fbxManager, const char* filename)
{
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");

	if (fbxImporter->Initialize(filename, -1, fbxManager->GetIOSettings()))
	{
		return FbxMeshDataImporter().Import(fbxManager, fbxImporter);
	}

	return nullptr;
}

void FBXEvaluateAnimForNode(FbxNode* node, Animation& anim, float startTime, float endTime, float frameRate)
{
	String name = node->GetName();

	FbxTime time;
	time.SetSecondDouble(0.0);

	float frameTime = 1.f / frameRate;

	AnimationTrack<Vector3>& translations = anim.GetTranslationTrack(name);
	AnimationTrack<Quaternion>& rotations = anim.GetRotationTrack(name);
	AnimationTrack<Vector3>& scales = anim.GetScalingTrack(name);

	for (float t = startTime; t < endTime; t += frameTime)
	{
		time.SetSecondDouble(t);
		translations.AddKey(t, FbxD3ToVector3(node->LclTranslation.EvaluateValue(time)));
		rotations.AddKey(t, FbxD3ToVector3(node->LclRotation.EvaluateValue(time)));
		scales.AddKey(t, FbxD3ToVector3(node->LclScaling.EvaluateValue(time)));
	}

	for (int i = 0; i < node->GetChildCount(); ++i)
		FBXEvaluateAnimForNode(node->GetChild(i), anim, startTime, endTime, frameRate);
}

FbxNode* FindSkeletonRoot(FbxNode* node)
{
	if (node->GetSkeleton())
		return node;

	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		FbxNode* foundNode = FindSkeletonRoot(node->GetChild(i));
		if (foundNode)
			return foundNode;
	}

	return nullptr;
}

Animation* EditorIO::ReadFBXAnimation(FbxManager* fbxManager, const char* filename)
{
	FbxImporter* importer = FbxImporter::Create(fbxManager, "");

	Skeleton skeleton;

	if (importer->Initialize(filename, -1, fbxManager->GetIOSettings()))
	{
		FbxScene* scene = FbxScene::Create(fbxManager, "importScene");

		importer->Import(scene);

		importer->Destroy();

		FbxNode* root = scene->GetRootNode();

		if (root)
		{
			FbxNode* skeletonRoot = FindSkeletonRoot(root);

			if (skeletonRoot)
			{
				Animation* anim = new Animation();

				auto ts = scene->GetSrcObject<FbxAnimStack>(0)->GetLocalTimeSpan();

				FBXEvaluateAnimForNode(skeletonRoot, *anim, 
					ts.GetStart().GetMilliSeconds() / 1000.f, ts.GetStop().GetMilliSeconds() / 1000.f, 30.f);
			
				return anim;
			}
		}


		scene->Destroy();
	}

	return nullptr;
}
