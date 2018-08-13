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
		[](GLModel &model) 
		{
			model.Delete(); 
		});

	_cube.Delete();
	_invCube.Delete();
	_plane.Delete();
	_basicPlane.Delete();
}

void ModelManager::Initialise()
{
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
		{ Vector3(-.5f, -.5f, 0), Vector2(0, 0),	planeColour, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1) },
		{ Vector3(.5f, -.5f, 0), Vector2(1, 0),		planeColour, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1) },
		{ Vector3(-.5f, .5f, 0), Vector2(0, 1),		planeColour, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1) },
		{ Vector3(.5f, .5f, 0), Vector2(1, 1),		planeColour, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1) }
	};

	uint32 planeElements[6] =
	{
		0, 1, 2,
		3, 2, 1
	};

	_plane.Create(planeVerts, 4, planeElements, 6);

	Vector3 basicPlaneVerts[4] =
	{
		Vector3(-.5f, -.5f, 0),
		Vector3(.5f, -.5f, 0),
		Vector3(-.5f, .5f, 0),
		Vector3(.5f, .5f, 0)
	};

	_basicPlane.Create(basicPlaneVerts, 4, planeElements, 6);
}

const GLModel& ModelManager::GetModel(const char *filename)
{
	GLModel& model = _models[filename];

	if (!model.IsValid())
	{
		ModelData data = IO::ReadOBJFile(filename);
		model.Create(data.vertices.Data(), data.vertices.GetSize(), data.elements.Data(), data.elements.GetSize());
	}

	return model;
}
