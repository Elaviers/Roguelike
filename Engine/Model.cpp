#include "Model.hpp"
#include "ColliderAABB.hpp"
#include "ColliderSphere.hpp"
#include "Engine.hpp"
#include "ModelManager.hpp"

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
			_mesh = Mesh::FromData(meshData);
		}

		if (_mesh)
			_mesh->CreateGLMeshRenderer(_meshRenderer);
	}
}

void Model::_CMD_collision(const Buffer<String>& args)
{
	if (args.GetSize() > 2)
	{
		if (args[0] == "sphere")
		{
			if (args.GetSize() >= 2)
			{
				delete this->_collider;
				this->_collider = new ColliderSphere(args[1].ToFloat());
			}
			else Debug::Error(CSTR("Insufficient sphere collision arguments"));
		}
		else if (args[0] == "aabb")
		{
			if (_mesh->IsValid())
			{
				delete _collider;
				_collider = new ColliderAABB(_mesh->bounds.min, _mesh->bounds.max);
			}
			else Debug::Error(CSTR("AABB collision cannot be used without specifying the model first!"));
		}
	}
}
