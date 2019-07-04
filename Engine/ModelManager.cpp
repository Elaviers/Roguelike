#include "ModelManager.hpp"
#include "ColliderAABB.hpp"
#include "ColliderSphere.hpp"
#include "Debug.hpp"
#include "IO.hpp"
#include "Utilities.hpp"
#include "Vector.hpp"

ModelManager::ModelManager() : AssetManager("")
{
}

ModelManager::~ModelManager()
{
	_DestroyResource(_line);
	_DestroyResource(_cube);
	_DestroyResource(_invCube);
	_DestroyResource(_plane);
}

Model* ModelManager::_CreateResource(const String& name, const String& data)
{
	Model* model = Asset::FromText<Model>(data);

	if (model == nullptr)
		Debug::Error(CSTR("Could not load model \"" + name + '\"'));

	return model;
}

void ModelManager::_DestroyResource(Model& model)
{
	model.Delete();
}


void ModelManager::Initialise()
{
	const Vector3 white(1.f, 1.f, 1.f);

	//Line

	Vertex17F lineVerts[2]
	{
		//pos						uv			colour		tangent			bitangent			normal
		{Vector3(0.f, -.5f, 0.f), Vector2(0, 0), white, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1)},
		{Vector3(0.f, .5f, 0.f), Vector2(1, 0), white, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1)}
	};

	//Plane

	Vertex17F planeVerts[4] =
	{
		{ Vector3(-.5f, -.5f, 0), Vector2(0, 0),	white, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1) },
		{ Vector3(.5f, -.5f, 0), Vector2(1, 0),		white, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1) },
		{ Vector3(-.5f, .5f, 0), Vector2(0, 1),		white, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1) },
		{ Vector3(.5f, .5f, 0), Vector2(1, 1),		white, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1) }
	};

	uint32 planeElements[6] =
	{
		0, 1, 2,
		3, 2, 1
	};

	//Cube

	Vertex17F cubeData[36] =
	{
		//Bottom
		VERT14F_TRI(Vector3(.5f, -.5f, -.5f),	Vector3(-.5f, -.5f, -.5f),	Vector3(.5f, -.5f, .5f),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(0, -1, 0)),
		VERT14F_TRI(Vector3(-.5f, -.5f, .5f),	Vector3(.5f, -.5f, .5f),	Vector3(-.5f, -.5f, -.5f),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(0, -1, 0)),

		//Top
		VERT14F_TRI(Vector3(-.5f, .5f, -.5f),	Vector3(.5f, .5f, -.5f),	Vector3(-.5f, .5f, .5f),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(0, 1, 0)),
		VERT14F_TRI(Vector3(.5f, .5f, .5f),		Vector3(-.5f, .5f, .5f),	Vector3(.5f, .5f, -.5f),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(0, 1, 0)),

		//Front
		VERT14F_TRI(Vector3(-.5f, -.5f, -.5f),	Vector3(.5f, -.5f, -.5f),	Vector3(-.5f, .5f, -.5f),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(0, 0, -1)),
		VERT14F_TRI(Vector3(.5f, .5f, -.5f),	Vector3(-.5f, .5f, -.5f),	Vector3(.5f, -.5f, -.5f),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(0, 0, -1)),

		//Right
		VERT14F_TRI(Vector3(.5f, -.5f, -.5f),	Vector3(.5f, -.5f, .5f),	Vector3(.5f, .5f, -.5f),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(1, 0, 0)),
		VERT14F_TRI(Vector3(.5f, .5f, .5f),		Vector3(.5f, .5f, -.5f),	Vector3(.5f, -.5f, .5f),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(1, 0, 0)),

		//Back
		VERT14F_TRI(Vector3(.5f, -.5f, .5f),	Vector3(-.5f, -.5f, .5f),	Vector3(.5f, .5f, .5f),		Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(0, 0, 1)),
		VERT14F_TRI(Vector3(-.5f, .5f, .5f),	Vector3(.5f, .5f, .5f),		Vector3(-.5f, -.5f, .5f),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(0, 0, 1)),

		//Left
		VERT14F_TRI(Vector3(-.5f, -.5f, .5f),	Vector3(-.5f, -.5f, -.5f),	Vector3(-.5f, .5f, .5f),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(-1, 0, 0)),
		VERT14F_TRI(Vector3(-.5f, .5f, -.5f),	Vector3(-.5f, .5f, .5f),	Vector3(-.5f, -.5f, -.5f),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(-1, 0, 0)),
	};

	for (int i = 0; i < 36; i += 3)
		Vertex17F::CalculateTangents(cubeData[i], cubeData[i + 1], cubeData[i + 2]);

	_line.MeshRenderer().SetDrawMode(GL_LINES);
	_line.MeshRenderer().Create(lineVerts, 2);
	_plane.MeshRenderer().Create(planeVerts, 4, planeElements, 6);
	_cube.MeshRenderer().Create(cubeData, 36);


	for (int i = 0; i < 36; i += 3) {
		cubeData[i].normal *= -1;
		cubeData[i + 1].normal *= -1;
		cubeData[i + 2].normal *= -1;

		Utilities::Swap(cubeData[i + 1], cubeData[i + 2]);
		Vertex17F::CalculateTangents(cubeData[i], cubeData[i + 1], cubeData[i + 2]);
	}
	_invCube.MeshRenderer().Create(cubeData, 36);
}
