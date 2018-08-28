#include "ModelManager.h"
#include "Error.h"
#include "IO.h"
#include "Utilities.h"
#include "Vector.h"

constexpr const char *extension = ".txt";

ModelManager::ModelManager()
{
}

ModelManager::~ModelManager()
{
	_map.ForEach(
		[](const String&, Model &model) 
		{
			model.model.Delete(); 
		});

	_cube.model.Delete();
	_invCube.model.Delete();
	_plane.model.Delete();
	_basicPlane.model.Delete();
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

	_cube.model.Create(cubeData, 36);

	for (int i = 0; i < 36; i += 3) {
		cubeData[i].normal *= -1;
		cubeData[i + 1].normal *= -1;
		cubeData[i + 2].normal *= -1;

		Utilities::Swap(cubeData[i + 1], cubeData[i + 2]);
		Vertex17F::CalculateTangents(cubeData[i], cubeData[i + 1], cubeData[i + 2]);
	}

	_invCube.model.Create(cubeData, 36);

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

	_plane.model.Create(planeVerts, 4, planeElements, 6);

	Vector3 basicPlaneVerts[4] =
	{
		Vector3(-.5f, -.5f, 0),
		Vector3(.5f, -.5f, 0),
		Vector3(-.5f, .5f, 0),
		Vector3(.5f, .5f, 0)
	};

	_basicPlane.model.Create(basicPlaneVerts, 4, planeElements, 6);

	_cube.defaultMaterial = _invCube.defaultMaterial = _plane.defaultMaterial = _basicPlane.defaultMaterial = "";
}

const Model* ModelManager::GetModel(const String &nameIn)
{
	String name = nameIn.ToLower();

	Model *model = _map.Find(name);
	if (model) return model;
	else
	{
		String fs = IO::ReadFileString((_rootPath + name + extension).ToLower().GetData());
		Utilities::LowerString(fs);
		Buffer<String> lines = fs.Split("\r\n");

		ModelData data;
		String materialName;

		for (uint32 i = 0; i < lines.GetSize(); ++i)
		{
			Buffer<String> tokens = lines[i].Split("=");

			if (tokens[0] == "model")
				data = IO::ReadOBJFile((_rootPath + tokens[1]).GetData());
			if (tokens[0] == "material")
				materialName = tokens[1];
		}

		if (data.IsValid())
		{
			Model &newModel = _map[name];
			newModel.model.Create(data.vertices.Data(), data.vertices.GetSize(), data.elements.Data(), data.elements.GetSize());
			newModel.defaultMaterial = materialName;
			return &newModel;
		}
	}
	
	return nullptr;
}
