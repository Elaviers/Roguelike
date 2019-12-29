#include "Model.hpp"
#include "Engine.hpp"
#include "MacroUtilities.hpp"
#include "ModelManager.hpp"

const PropertyCollection& Model::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN
	properties.AddCommand("model", MemberCommandPtr<Model>(&Model::_CMD_model));
	properties.AddCommand("collision", MemberCommandPtr<Model>(&Model::_CMD_collision));
	properties.Add<String>("material", offsetof(Model, _defaultMaterialName));
	DO_ONCE_END

	return properties;
}

void Model::_CMD_model(const Buffer<String> &args)
{
	if (args.GetSize() > 0)
	{
		String ext = Utilities::GetExtension(args[0]).ToLower();
		String path = Engine::Instance().pModelManager->GetRootPath() + args[0];

		if (ext == ".obj")
			_mesh = IO::ReadOBJFile(path.GetData());
		else
		{
			Buffer<byte> meshData = IO::ReadFile(path.GetData());

			if (meshData.GetSize() == 0)
				return;

			_mesh = Mesh::FromData(meshData);
		}

		if (_mesh)
			_mesh->CreateGLMeshRenderer(_meshRenderer);
	}
}

#include "CollisionBox.hpp"
#include "CollisionCapsule.hpp"
#include "CollisionSphere.hpp"

void Model::_CMD_collision(const Buffer<String>& args)
{
	if (args.GetSize() > 0)
	{
		if (args[0] == "sphere")
		{
			if (args.GetSize() >= 2)
			{
				_collider = Collider(CollisionChannels::SURFACE, CollisionSphere(args[1].ToFloat()));
			}
			else Debug::Error("Insufficient sphere collision arguments");
		}
		else if (args[0] == "box")
		{
			if (_mesh && _mesh->IsValid())
			{
				_collider = Collider(CollisionChannels::SURFACE, CollisionBox(Box::FromMinMax(_mesh->bounds.min, _mesh->bounds.max)));
			}
			else Debug::Error("Box collision cannot be used without specifying the model first!");
		}
		else if (args[0] == "capsule")
		{
			if (args.GetSize() >= 3)
			{
				_collider = Collider(CollisionChannels::SURFACE, CollisionCapsule(args[1].ToFloat(), args[2].ToFloat()));
			}
			else Debug::Error("Insufficient capsule collision arguments");
		}
	}
}
