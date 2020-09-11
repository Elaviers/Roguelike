#include "ModelManager.hpp"
#include <ELGraphics/MeshManager.hpp>
#include <ELPhys/CollisionBox.hpp>

Model* ModelManager::_CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context& ctx)
{
	Model* model = Asset::FromText<Model>(data, ctx);
	if (model == nullptr)
		Debug::Error(CSTR("Could not load model \"", name, "\""));

	return model;
}

void ModelManager::Initialise(MeshManager& meshManager)
{
	_line.SetMesh(meshManager.Line());
	_plane.SetMesh(meshManager.Plane());
	_cube.SetMesh(meshManager.Cube());
	_invCube.SetMesh(meshManager.InverseCube());

	_plane.SetCollider(Collider(ECollisionChannels::ALL, CollisionBox(Transform(Vector3(), Vector3(), Vector3(1, 1, .001f)))));
	_cube.SetCollider(Collider(ECollisionChannels::ALL, CollisionBox(Box(Vector3(), Vector3(.5f, .5f, .5f)))));

	SharedPointerData<Model>& dLine = _MapValue("line"), &dPlane = _MapValue("plane"), &dCube = _MapValue("cube"), &dInvCube = _MapValue("invcube");
	dLine.SetPtr(&_line);
	dPlane.SetPtr(&_plane);
	dCube.SetPtr(&_cube);
	dInvCube.SetPtr(&_invCube);

	_tLine = SharedPointer<Model>(dLine);
	_tPlane = SharedPointer<Model>(dPlane);
	_tCube = SharedPointer<Model>(dCube);
	_tInvCube = SharedPointer<Model>(dInvCube);
}
