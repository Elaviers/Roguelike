#include "MeshManager.hpp"
#include "IO.hpp"

Mesh* MeshManager::_CreateResource(const Buffer<byte>& data, const String& name, const String& ext, const Context& ctx)
{
	Mesh* mesh = Mesh::FromData(data);

	if (mesh == nullptr)
		Debug::Error(CSTR("Could not load mesh \"", name, "\""));

	return Mesh::FromData(data);
}

Mesh* MeshManager::_CreateResource(const String& data, const String& name, const String& ext, const Context& ctx)
{
	if (ext == ".obj")
	{
		Mesh* mesh = IO::ReadOBJFile(data);

		if (mesh == nullptr)
			Debug::Error(CSTR("Could not load OBJ \"", name, "\""));

		return mesh;
	}

	return nullptr;
}

void MeshManager::Initialise()
{
	const Vector3 white(1.f, 1.f, 1.f);

	//Line
	_line = new Mesh_Static(
		Buffer<Vertex17F>({
			//pos						uv			_colour		tangent			bitangent			normal
			{Vector3(0.f, -.5f, 0.f), Vector2(0, 0), white, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1)},
			{Vector3(0.f, .5f, 0.f), Vector2(1, 0), white, Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1)}
			})
	);

	//Plane
	_plane = new Mesh_Static(
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

	Vertex17F::CalculateTangents(_plane->vertices[0], _plane->vertices[1], _plane->vertices[2]);
	Vertex17F::CalculateTangents(_plane->vertices[3], _plane->vertices[2], _plane->vertices[1]);

	//Cube
	_cube = new Mesh_Static(
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
		Vertex17F::CalculateTangents(_cube->vertices[i], _cube->vertices[i + 1], _cube->vertices[i + 2]);

	_line->bounds = Bounds(Vector3(0.f, 0.5f, 0.f));
	_plane->bounds = Bounds(Vector3(.5f, .5f, 0.f));
	_cube->bounds = Bounds(Vector3(.5f, .5f, .5f));

	_line->SetRenderMode(ERenderMode::LINES);
	_line->UpdateRenderer();
	_plane->UpdateRenderer();
	_cube->UpdateRenderer();
	
	_invCube = new Mesh_Static(_cube->vertices, _cube->elements);
	_invCube->bounds = _cube->bounds;

	for (size_t i = 0; i < 36; i += 3) {
		_invCube->vertices[i].normal *= -1;
		_invCube->vertices[i + 1].normal *= -1;
		_invCube->vertices[i + 2].normal *= -1;

		Utilities::Swap(_invCube->vertices[i + 1], _invCube->vertices[i + 2]);
		Vertex17F::CalculateTangents(_invCube->vertices[i], _invCube->vertices[i + 1], _invCube->vertices[i + 2]);
	}

	_invCube->UpdateRenderer();

	SharedPointerData<Mesh>& dLine = _MapValue("line"), & dPlane = _MapValue("plane"), & dCube = _MapValue("cube"), & dInvCube = _MapValue("invcube");

	dLine.SetPtr(_line);
	dPlane.SetPtr(_plane);
	dCube.SetPtr(_cube);
	dInvCube.SetPtr(_invCube);

	_tLine = SharedPointer<Mesh>(dLine);
	_tPlane = SharedPointer<Mesh>(dPlane);
	_tCube = SharedPointer<Mesh>(dCube);
	_tInvCube = SharedPointer<Mesh>(dInvCube);
}
