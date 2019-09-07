#include "Model.hpp"
#include "ColliderBox.hpp"
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

			if (meshData.GetSize() == 0)
				return;

			_mesh = Mesh::FromData(meshData);
		}

		if (_mesh)
			_mesh->CreateGLMeshRenderer(_meshRenderer);
	}
}

void Model::_CMD_collision(const Buffer<String>& args)
{
	if (args.GetSize() > 0)
	{
		if (args[0] == "sphere")
		{
			if (args.GetSize() >= 2)
			{
				delete this->_collider;
				this->_collider = new ColliderSphere(COLL_SURFACE, args[1].ToFloat());
			}
			else Debug::Error("Insufficient sphere collision arguments");
		}
		else if (args[0] == "box")
		{
			if (_mesh && _mesh->IsValid())
			{
				delete _collider;
				_collider = new ColliderBox(COLL_SURFACE, Box::FromMinMax(_mesh->bounds.min, _mesh->bounds.max));
			}
			else Debug::Error("Box collision cannot be used without specifying the model first!");
		}
	}
}
