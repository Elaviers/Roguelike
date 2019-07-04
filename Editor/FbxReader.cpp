#include "EditorIO.hpp"
#include <Engine/Animation.hpp>
#include <Engine/Debug.hpp>
#include <Engine/Map.hpp>
#include <Engine/Mesh_Skeletal.hpp>
#include <Engine/Mesh_Static.hpp>

inline Vector3 FbxVec4ToVector3(const FbxVector4 &v)
{
	return Vector3(-v[0], v[2], -v[1]);
}

inline Vector3 FbxD3ToVector3(const FbxDouble3& v)
{
	return Vector3(-v[0], v[2], -v[1]);
}

inline Vector2 FbxVec2ToVector2(const FbxVector2& v)
{
	return Vector2(v[0], v[1]);
}

Vertex17F GetFBXVertex(const FbxMesh* fbxMesh, int polygonIndex, int vertexIndex, const char *uvSet)
{
	Vertex17F v = Vertex17F();
	int control = fbxMesh->GetPolygonVertex(polygonIndex, vertexIndex);
	
	if (control > -1)
		v.pos = FbxVec4ToVector3(fbxMesh->mControlPoints[control]);
	
	FbxVector4 normal;
	if (fbxMesh->GetPolygonVertexNormal(polygonIndex, vertexIndex, normal))
		v.normal = FbxVec4ToVector3(normal);

	FbxVector2 uv;
	bool unmapped;
	if (fbxMesh->GetPolygonVertexUV(polygonIndex, vertexIndex, uvSet, uv, unmapped))
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

inline uint32 GetVertexIndex(Buffer<Vertex17F>& vertices, uint32& vertexCount, const FbxMesh *fbxMesh, int polyIndex, int vertIndex, const char *uvSet)
{
	return GetVertexIndex(vertices, vertexCount, GetFBXVertex(fbxMesh, polyIndex, vertIndex, uvSet));
}

void LoadMesh(Buffer<Vertex17F> &vertices, Buffer<uint32> &elements, Map<String, Buffer<uint32>> &vertexMappings, FbxMesh* fbxMesh)
{
	const char* uvSet = nullptr;

	FbxStringList uvSetNames;
	fbxMesh->GetUVSetNames(uvSetNames);
	if (uvSetNames.GetCount() > 0)
		uvSet = uvSetNames[0];

	uint32 vertexCount = 0;

	int polyCount = fbxMesh->GetPolygonCount();
	for (int i = 0; i < polyCount; ++i)
	{
		int polySize = fbxMesh->GetPolygonSize(i);

		if (polySize <= 2)
		{
			Debug::PrintLine("Warning: FBX polygon with less than 3 vertices?!, ignoring...");
			continue;
		}

		uint32 v1 = GetVertexIndex(vertices, vertexCount, fbxMesh, i, 0, uvSet);
		uint32 v2 = GetVertexIndex(vertices, vertexCount, fbxMesh, i, 1, uvSet);
		uint32 v3 = GetVertexIndex(vertices, vertexCount, fbxMesh, i, 2, uvSet);

		size_t last = elements.GetSize();
		elements.Append(3);

		elements[last] = v1;
		elements[last + 1] = v2;
		elements[last + 2] = v3;
		Vertex17F::CalculateTangents(vertices[v1], vertices[v3], vertices[v2]);

		if (polySize > 3)	//Quad, add another triangle
		{
			uint32 v4 = GetVertexIndex(vertices, vertexCount, fbxMesh, i, 3, uvSet);

			size_t last = elements.GetSize();
			elements.Append(3);

			elements[last] = v1;
			elements[last + 1] = v3;
			elements[last + 2] = v4;
			Vertex17F::CalculateTangents(vertices[v1], vertices[v4], vertices[v3]);

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
					
					Buffer<uint32>& dest = vertexMappings[cluster->GetName()];
					dest.SetSize(cpIndexCount);
					for (int k = 0; k < cpIndexCount; ++k)
						dest[k] = cpIndices[k];
				}
			}
			else Debug::Error("poop error");
		}
	}
}

void LoadSkeleton(FbxNode* node, Skeleton &skeleton, Joint* parentJoint = nullptr)
{
	Joint* joint = skeleton.CreateJoint(parentJoint);

	if (joint == nullptr)
	{
		Debug::Error("Bone error");
		return;
	}

	joint->name = node->GetSkeleton()->GetName();
	FbxTransform &fbxt = node->GetTransform();

	for (int i = 0; i < node->GetChildCount(); ++i)
		LoadSkeleton(node->GetChild(i), skeleton, joint);

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

void LoadNodeChildren(FbxNode *parent, 
	Buffer<Vertex17F>& vertices, Buffer<uint32>& elements, Map<String, Buffer<uint32>>& vertexMappings, Skeleton &skeleton, FbxNode*& skeletonNode,
	FbxTransform *parentTransform = nullptr)
{
	for (int i = 0; i < parent->GetChildCount(); ++i)
	{
		FbxNode* node = parent->GetChild(i);

		FbxMesh* fbxMesh = node->GetMesh();

		if (fbxMesh)
			LoadMesh(vertices, elements, vertexMappings, fbxMesh);

		if (node->GetSkeleton())
		{
			skeleton.Clear();

			LoadSkeleton(node, skeleton);
			skeletonNode = node;
			continue;
		}

		LoadNodeChildren(node, vertices, elements, vertexMappings, skeleton, skeletonNode);
	}
}

Mesh* EditorIO::ReadFBXMesh(FbxManager *fbxManager, const char* filename)
{
	FbxImporter* importer = FbxImporter::Create(fbxManager, "");

	Buffer<Vertex17F> verts;
	Buffer<uint32> elements;
	Map<String, Buffer<uint32>> vertexMappings;
	Skeleton skeleton;

	if (importer->Initialize(filename, -1, fbxManager->GetIOSettings()))
	{
		FbxScene* scene = FbxScene::Create(fbxManager, "importScene");

		importer->Import(scene);

		importer->Destroy();

		FbxNode* skeletonNode = nullptr;
		FbxNode* root = scene->GetRootNode();

		if (root)
		{
			LoadNodeChildren(root, verts, elements, vertexMappings, skeleton, skeletonNode);
		}
		

		scene->Destroy();
	}

	if (verts.GetSize())
	{
		if (!vertexMappings.IsEmpty() && skeleton.GetJointCount())
		{
			Mesh_Skeletal* skeletalMesh = new Mesh_Skeletal();

			Buffer<uint32> nextBoneIndices;
			skeletalMesh->vertices.SetSize(verts.GetSize());
			nextBoneIndices.SetSize(verts.GetSize());

			for (int i = 0; i < nextBoneIndices.GetSize(); ++i)
				nextBoneIndices[i] = 0;

			for (size_t i = 0; i < skeletalMesh->vertices.GetSize(); ++i)
			{
				skeletalMesh->vertices[i].pos = verts[i].pos;
				skeletalMesh->vertices[i].normal = verts[i].normal;
				skeletalMesh->vertices[i].tangent = verts[i].tangent;
				skeletalMesh->vertices[i].bitangent = verts[i].bitangent;
				skeletalMesh->vertices[i].uvOffset = verts[i].uvOffset;
			}

			for (auto node = skeleton.FirstListNode(); node; node = node->next)
			{
				Joint& j = node->obj;

				Buffer<uint32> *vertsWithJoint = vertexMappings.Get(j.name);

				if (vertsWithJoint)
				{
					int jointID = j.GetID();

					for (int i = 0; i < vertsWithJoint->GetSize(); ++i)
					{
						uint32 boneIndex = nextBoneIndices[i]++;

						if (boneIndex < 2)
						{
							skeletalMesh->vertices[(*vertsWithJoint)[i]].boneIndices[boneIndex] = jointID;
							skeletalMesh->vertices[(*vertsWithJoint)[i]].boneWeights[boneIndex] = 1.f;
						}
					}
				}
			}

			skeletalMesh->elements = std::move(elements);
			skeletalMesh->skeleton = std::move(skeleton);

			skeletalMesh->bounds.RecalculateSphereBounds();
			
			return skeletalMesh;
		}
		
		{
			Mesh_Static* staticMesh = new Mesh_Static();

			staticMesh->vertices = std::move(verts);

			staticMesh->elements = std::move(elements);
			staticMesh->bounds.RecalculateSphereBounds();

			return staticMesh;
		}
	}

	return nullptr;
}

void FBXEvaluateAnimForNode(FbxNode* node, Animation& anim, float startTime, float endTime, float frameRate)
{
	String name = node->GetName();

	float frameTime = 1.f / frameRate;

	auto translations = anim.GetTranslationTrack(name);
	auto rotations = anim.GetRotationTrack(name);
	auto scales = anim.GetScalingTrack(name);

	for (float t = startTime; t < endTime; t += frameTime)
	{
		translations->AddKey(t, FbxD3ToVector3(node->LclTranslation.EvaluateValue(t)));
		rotations->AddKey(t, FbxD3ToVector3(node->LclRotation.EvaluateValue(t)));
		scales->AddKey(t, FbxD3ToVector3(node->LclScaling.EvaluateValue(t)));
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

		FbxNode* skeletonNode = nullptr;
		FbxNode* root = scene->GetRootNode();

		if (root)
		{
			FbxNode* skeletonRoot = FindSkeletonRoot(root);

			if (skeletonRoot)
			{
				Animation* anim = new Animation();

				auto ts = scene->GetSrcObject<FbxAnimStack>(0)->GetLocalTimeSpan();

				FBXEvaluateAnimForNode(skeletonNode, *anim, ts.GetStart().GetMilliSeconds() / 1000.f, ts.GetStop().GetMilliSeconds() / 1000.f, 30.f);
			
				return anim;
			}
		}


		scene->Destroy();
	}

	return nullptr;
}
