#include "Model.hpp"
#include "ColliderAABB.hpp"
#include "ColliderSphere.hpp"
#include "Engine.hpp"
#include "ModelManager.hpp"

void Model::_CMD_model(const Buffer<String> &args)
{
	if (args.GetSize() > 0)
	{
		_data = IO::ReadOBJFile((Engine::Instance().pModelManager->GetRootPath() + args[0]).GetData());
	
		this->model.Create(_data.vertices.Data(), (GLsizei)_data.vertices.GetSize(), _data.elements.Data(), (GLsizei)_data.elements.GetSize());
		this->bounds = _data.bounds;
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
				delete this->collider;
				this->collider = new ColliderSphere(args[1].ToFloat());
			}
			else Debug::Error(CSTR("Insufficient sphere collision arguments"));
		}
		else if (args[0] == "aabb")
		{
			if (_data.IsValid())
			{
				delete collider;
				collider = new ColliderAABB(_data.bounds.min, _data.bounds.max);
			}
			else Debug::Error(CSTR("AABB collision cannot be used without specifying the model first!"));
		}
	}
}

void Model::FromString(const String& data)
{
	Buffer<String> lines = data.ToLower().Split("\r\n");

	for (size_t i = 0; i < lines.GetSize(); ++i)
		_cvars.HandleCommand(lines[i]);


	//Cleanup, these aren't needed now
	_data.vertices.SetSize(0);
	_data.elements.SetSize(0);
}
