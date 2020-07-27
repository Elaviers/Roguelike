#pragma once
#include "Entity.hpp"
#include <ELCore/Context.hpp>
#include <ELGraphics/Colour.hpp>
#include <ELGraphics/MaterialManager.hpp>
#include <ELGraphics/Material_Sprite.hpp>
#include <ELPhys/CollisionSphere.hpp>

class EntSprite : public Entity
{
	SharedPointer<const MaterialSprite> _material;
	float _size;
	Colour _colour;

	bool _fixedYaw;

public:
	Entity_FUNCS(EntSprite, EEntityID::SPRITE)

	EntSprite(const MaterialSprite *material = nullptr, float size = 1.f) : 
		_material(material), 
		_size(size),
		_colour(1.f, 1.f, 1.f),
		_fixedYaw(true)
	{}
	virtual ~EntSprite() {}

	float GetSize() const { return _size; }
	const Colour& GetColour() const { return _colour; }

	void SetSize(float size) { _size = size; SetWorldScale(Vector3(size, size, size)); }
	void SetColour(const Colour& colour) { _colour = colour; }

	virtual void Render(RenderQueue&) const override;

	virtual Bounds GetBounds() const override { return Bounds(_size / 2.f); }
	virtual const Collider* GetCollider() const override 
	{ 
		static Collider editorCollider(ECollisionChannels::EDITOR, CollisionSphere(0.5f)); 
		return &editorCollider;
	}

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
	virtual void WriteData(ByteWriter&, NumberedSet<String>& strings, const Context& ctx) const override;
	virtual void ReadData(ByteReader&, const NumberedSet<String>& strings, const Context& ctx) override;
};

