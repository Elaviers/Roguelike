#include "Tile.hpp"
#include <ELGraphics/MaterialManager.hpp>
#include <ELGraphics/MeshManager.hpp>
#include <ELPhys/CollisionBox.hpp>

const PropertyCollection& Tile::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.Add("collision", MemberGetter<Tile, String>(&Tile::_GetCollisionTypeStr), MemberSetter<Tile, String>(&Tile::_SetCollisionTypeStr));
	properties.Add("material", ContextualMemberGetter<Tile, String>(&Tile::_GetMaterialStr), ContextualMemberSetter<Tile, String>(&Tile::_SetMaterialStr));
	properties.Add("mesh", ContextualMemberGetter<Tile, String>(&Tile::_GetMeshStr), ContextualMemberSetter<Tile, String>(&Tile::_SetMeshStr));
	properties.Add("size", MemberGetter<Tile, const Vector2&>(&Tile::_GetSize), MemberSetter<Tile, Vector2>(&Tile::_SetSize));
	properties.Add("extra", MemberGetter<Tile, const Vector2&>(&Tile::_GetExtra), MemberSetter<Tile, Vector2>(&Tile::_SetExtra));
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
	SetMaterial(materialManager->Get(string, ctx));
}

String Tile::_GetMaterialStr(const Context& ctx)
{
	MaterialManager* materialManager = ctx.GetPtr<MaterialManager>();
	return materialManager->FindNameOf(_material.Ptr());
}

void Tile::_SetMeshStr(const String& string, const Context& ctx)
{
	MeshManager* meshManager = ctx.GetPtr<MeshManager>();
	_mesh = meshManager->Get(string, ctx);
}

String Tile::_GetMeshStr(const Context& ctx)
{
	MeshManager* meshManager = ctx.GetPtr<MeshManager>();
	return meshManager->FindNameOf(_mesh.Ptr());
}

void Tile::_UpdateRenderSize()
{
	_renderSize = _size;
	
	if (_material && (_extra.x || _extra.y))
	{
		const SharedPointer<const Texture>& t = _material->GetDefaultTexture();

		if (t)
		{
			//law of cosines
			float hyp = Maths::SquareRoot(3.f * _renderSize.x * _renderSize.x);

			float w = (float)t->GetWidth();
			float h = (float)t->GetHeight();

			hyp *= w / (w - _extra.x);

			_renderSize.x *= Maths::SquareRoot((hyp * hyp) / 3.f);
			_renderSize.y *= h / (h - _extra.y);
		}
	}
}

const Collider& Tile::GetCollider() const
{
	static ECollisionChannels channels = ECollisionChannels::PLAYER | ECollisionChannels::STATIC;
	static Collider block(channels, CollisionBox(Box::FromMinMax(Vector3(0.f, 0.f, 0.f), Vector3(1.f, 1.f, 1.f))));
	static Collider wallL(channels, CollisionBox(Box::FromMinMax(Vector3(0.f, 0.f, 0.f), Vector3(.2f, 1.f, 1.f))));
	static Collider wallR(channels, CollisionBox(Box::FromMinMax(Vector3(0.f, 0.f, 0.f), Vector3(1.f, 1.f, .2f))));
	static Collider none;

	switch (_collisionType)
	{
	case ECollisionType::BLOCK:
		return block;
	case ECollisionType::WALL_L:
		return wallL;
	case ECollisionType::WALL_R:
		return wallR;
	}

	return none;
}
