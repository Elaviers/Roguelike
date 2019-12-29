#include "ModelManager.hpp"
#include "CollisionBox.hpp"
#include "Debug.hpp"
#include "IO.hpp"
#include "Utilities.hpp"
#include "Vector.hpp"

ModelManager::ModelManager() : AssetManager()
{
}

ModelManager::~ModelManager()
{
}

Model* ModelManager::_CreateResource(const String& name, const String& data)
{
	Model* model = Asset::FromText<Model>(data);

	if (model == nullptr)
		Debug::Error(CSTR("Could not load model \"", name, '\"'));

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
	Mesh_Static* lineMesh = new Mesh_Static(
		Buffer<Vertex17F>({
			//pos						uv			colour		tangent			bitangent			normal
			{Vector3(0.f, -.5f, 0.f), Vector2(0, 0), white, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1)},
			{Vector3(0.f, .5f, 0.f), Vector2(1, 0), white, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1)}
		})
	);

	//Plane
	Mesh_Static* planeMesh = new Mesh_Static(
		Buffer<Vertex17F>({
			{ Vector3(-.5f, -.5f, 0), Vector2(0, 0),	white, Vector3(), Vector3(), Vector3(0, 0, -1) },
			{ Vector3(.5f, -.5f, 0), Vector2(1, 0),		white, Vector3(), Vector3(), Vector3(0, 0, -1) },
			{ Vector3(-.5f, .5f, 0), Vector2(0, 1),		white, Vector3(), Vector3(), Vector3(0, 0, -1) },
			{ Vector3(.5f, .5f, 0), Vector2(1, 1),		white, Vector3(), Vector3(), Vector3(0, 0, -1) }
		}),
		Buffer<uint32>({
			0, 1, 2,
			3, 2, 1
		}));
	
	Vertex17F::CalculateTangents(planeMesh->vertices[0], planeMesh->vertices[1], planeMesh->vertices[2]);
	Vertex17F::CalculateTangents(planeMesh->vertices[3], planeMesh->vertices[2], planeMesh->vertices[1]);

	//Cube
	Mesh_Static* cubeMesh = new Mesh_Static(
		Buffer<Vertex17F>({
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
			VERT14F_TRI(Vector3(-.5f, .5f, -.5f),	Vector3(-.5f, .5f, .5f),	Vector3(-.5f, -.5f, -.5f),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(-1, 0, 0))
		})
	);

	for (size_t i = 0; i < 36; i += 3)
		Vertex17F::CalculateTangents(cubeMesh->vertices[i], cubeMesh->vertices[i + 1], cubeMesh->vertices[i + 2]);

	lineMesh->bounds = Bounds(Vector3(0.f, 0.5f, 0.f));
	planeMesh->bounds = Bounds(Vector3(.5f, .5f, 0.f));
	cubeMesh->bounds = Bounds(Vector3(.5f, .5f, .5f));

	_line = new Model(lineMesh, Collider(CollisionChannels::SURFACE, CollisionBox(Box::FromMinMax(lineMesh->bounds.min, lineMesh->bounds.max))));
	_plane = new Model(planeMesh, Collider(CollisionChannels::SURFACE, CollisionBox(Box::FromMinMax(planeMesh->bounds.min, planeMesh->bounds.max))));
	_cube = new Model(cubeMesh, Collider(CollisionChannels::SURFACE, CollisionBox(Box::FromMinMax(cubeMesh->bounds.min, cubeMesh->bounds.max))));

	_line->MeshRenderer().SetDrawMode(GL_LINES);
	lineMesh->CreateGLMeshRenderer(_line->MeshRenderer());
	planeMesh->CreateGLMeshRenderer(_plane->MeshRenderer());
	
	cubeMesh->CreateGLMeshRenderer(_cube->MeshRenderer());

	Mesh_Static* invCubeMesh = new Mesh_Static(*cubeMesh);
	invCubeMesh->bounds = cubeMesh->bounds;

	for (size_t i = 0; i < 36; i += 3) {
		invCubeMesh->vertices[i].normal *= -1;
		invCubeMesh->vertices[i + 1].normal *= -1;
		invCubeMesh->vertices[i + 2].normal *= -1;

		Utilities::Swap(invCubeMesh->vertices[i + 1], invCubeMesh->vertices[i + 2]);
		Vertex17F::CalculateTangents(invCubeMesh->vertices[i], invCubeMesh->vertices[i + 1], invCubeMesh->vertices[i + 2]);
	}

	_invCube = new Model(invCubeMesh, Collider(CollisionChannels::SURFACE, CollisionBox(Box::FromMinMax(invCubeMesh->bounds.min, invCubeMesh->bounds.max))));
	invCubeMesh->CreateGLMeshRenderer(_invCube->MeshRenderer());

	SharedPointerData<Model> &dLine = _MapValue("line"), &dPlane = _MapValue("plane"), &dCube = _MapValue("cube"), &dInvCube = _MapValue("invcube");

	dLine.SetPtr(_line);
	dPlane.SetPtr(_plane);
	dCube.SetPtr(_cube);
	dInvCube.SetPtr(_invCube);

	_tLine = SharedPointer<Model>(dLine);
	_tPlane = SharedPointer<Model>(dPlane);
	_tCube = SharedPointer<Model>(dCube);
	_tInvCube = SharedPointer<Model>(dInvCube);
}
