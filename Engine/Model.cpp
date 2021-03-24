#include "Model.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/MacroUtilities.hpp>
#include <ELGraphics/MeshManager.hpp>
#include <ELGraphics/IO.hpp>

const PropertyCollection& Model::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN
		properties.AddCommand("mesh", MemberCommandPtr<Model>(&Model::_CMD_mesh));
		properties.AddCommand("collision", MemberCommandPtr<Model>(&Model::_CMD_collision));
		properties.Add<String>("material", offsetof(Model, _defaultMaterialName));
	DO_ONCE_END

	return properties;
}

void Model::_CMD_mesh(const Buffer<String>& args, const Context& ctx)
{
	if (args.GetSize() > 0)
	{
		_mesh = ctx.GetPtr<MeshManager>()->Get(args[0], ctx);
	}
}

#include <ELPhys/CollisionBox.hpp>
#include <ELPhys/CollisionCapsule.hpp>
#include <ELPhys/CollisionSphere.hpp>

void Model::_CMD_collision(const Buffer<String>& args, const Context& ctx)
{
	if (args.GetSize() > 0)
	{
		if (args[0] == "sphere")
		{
			if (args.GetSize() >= 2)
			{
				_collider = Collider(ECollisionChannels::SURFACE, CollisionSphere(args[1].ToFloat()));
			}
			else Debug::Error("Insufficient sphere collision arguments");
		}
		else if (args[0] == "box")
		{
			if (_mesh && _mesh->IsValid())
			{
				Vector3 min, max;
				_mesh->GetVolume()->Bounds_AABB(Transform::Identity(), min, max);

				_collider = Collider(ECollisionChannels::SURFACE, CollisionBox(Box::FromMinMax(min, max)));
			}
			else Debug::Error("Box collision cannot be used without specifying the mesh first!");
		}
		else if (args[0] == "capsule")
		{
			if (args.GetSize() >= 3)
			{
				_collider = Collider(ECollisionChannels::SURFACE, CollisionCapsule(args[1].ToFloat(), args[2].ToFloat()));
			}
			else Debug::Error("Insufficient capsule collision arguments");
		}
	}
}
