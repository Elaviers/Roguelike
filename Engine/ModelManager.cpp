#include "ModelManager.hpp"
#include <ELCore/Context.hpp>
#include <ELGraphics/MeshManager.hpp>
#include <ELPhys/CollisionBox.hpp>

Model* ModelManager::_CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context& ctx)
{
	Model* model = Asset::FromText<Model>(data, ctx);
	if (model == nullptr)
		Debug::Error(CSTR("Could not load model \"", name, "\""));

	return model;
}

bool ModelManager::_CreateAlternative(Model*& resource, const String& name, const Context& ctx)
{
	if (resource)
	{
		SharedPointer<const Mesh> mesh = ctx.GetPtr<MeshManager>()->Get(name, ctx);
		if (mesh)
		{
			resource->SetMesh(mesh);
			return true;
		}
	}

	return false;
}


Buffer<const AssetManagerBase*> ModelManager::_GetFallbackManagers(const Context& ctx) const
{
	return { (const AssetManagerBase*)ctx.GetPtr<MeshManager>() };
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
