#pragma once
#include "WorldObject.hpp"
#include <ELCore/Context.hpp>
#include <ELGraphics/Colour.hpp>
#include <ELGraphics/MaterialManager.hpp>
#include <ELGraphics/Material_Sprite.hpp>

class OSprite : public WorldObject
{
	SharedPointer<const MaterialSprite> _material;
	float _size;
	Colour _colour;

	bool _fixedYaw;

protected:
	OSprite(World& world) : WorldObject(world), _size(1.f), _colour(Colour::White), _fixedYaw(true) {}

public:
	WORLDOBJECT_VFUNCS(OSprite, EObjectID::SPRITE);

	virtual ~OSprite() {}

	float GetSize() const { return _size; }
	const Colour& GetColour() const { return _colour; }

	void SetSize(float size) { _size = size; SetAbsoluteScale(Vector3(size, size, size)); }
	void SetColour(const Colour& colour) { _colour = colour; }

	virtual void Render(RenderQueue&) const override;

	virtual bool IsVisible(const Frustum& view) const override;
	
	virtual const PropertyCollection& GetProperties() override;

	SharedPointer<const MaterialSprite> GetMaterial() const { return _material; }

	String GetMaterialName(const Context& ctx) const
	{
		return ctx.GetPtr<MaterialManager>()->FindNameOf(_material.Ptr());
	}

	void SetMaterialName(const String& name, const Context& ctx)
	{
		_material = ctx.GetPtr<MaterialManager>()->Get(name, ctx).Cast<const MaterialSprite>();
	}

	void SetMaterial(const SharedPointer<const MaterialSprite>& material)
	{
		_material = material;
	}

	//File IO
	virtual void Read(ByteReader& buffer, ObjectIOContext& ctx) override;
	virtual void Write(ByteWriter& buffer, ObjectIOContext& ctx) const override;
};
