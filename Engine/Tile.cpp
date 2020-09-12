#include "Tile.hpp"
#include <ELGraphics/MaterialManager.hpp>

const PropertyCollection& Tile::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.Add("collision", MemberGetter<Tile, String>(&Tile::_GetCollisionTypeStr), MemberSetter<Tile, String>(&Tile::_SetCollisionTypeStr));
	properties.Add("material", ContextualMemberGetter<Tile, String>(&Tile::_GetMaterialStr), ContextualMemberSetter<Tile, String>(&Tile::_SetMaterialStr));
	properties.Add<Vector2>("size", offsetof(Tile, _size));
	DO_ONCE_END;

	return properties;
}

void Tile::_SetCollisionTypeStr(const String& string)
{
	String s = string.ToLower();
	if (s == "block")
		_collisionType = ECollisionType::BLOCK;
	else if (s == "wall_l")
		_collisionType = ECollisionType::WALL_L;
	else if (s == "wall_r")
		_collisionType = ECollisionType::WALL_R;
	else if (s == "none")
		_collisionType = ECollisionType::NONE;
	else
		Debug::Message("Warning", CSTR('\"', string, "\" is not a tile collision type!"));
}

String Tile::_GetCollisionTypeStr() const
{
	switch (_collisionType)
	{
	case ECollisionType::BLOCK:
		return "block";
	case ECollisionType::WALL_L:
		return "wall_l";
	case ECollisionType::WALL_R:
		return "wall_r";
	case ECollisionType::NONE:
		return "none";

	default:
		Debug::Break(); //Should never execute
		return "???";
	}
}

void Tile::_SetMaterialStr(const String& string, const Context& ctx)
{
	MaterialManager* materialManager = ctx.GetPtr<MaterialManager>();
	_material = materialManager->Get(string, ctx);
}

String Tile::_GetMaterialStr(const Context& ctx)
{
	MaterialManager* materialManager = ctx.GetPtr<MaterialManager>();
	return materialManager->FindNameOf(_material.Ptr());
}