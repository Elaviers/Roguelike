#pragma once
#include "EntRenderable.hpp"
#include <ELPhys/CollisionBox.hpp>

class EntBrush2D : public EntRenderable
{
protected:
	Vector2 _point1;
	Vector2 _point2;

	void _OnTransformChanged();
	void _OnPointChanged();

	bool _updatingTransform;

public:
	Entity_FUNCS(EntBrush2D, EEntityID::PLANE)

	float level;

	EntBrush2D() : _updatingTransform(false), level(0.f) 
	{ 
		onTransformChanged += Callback(this, &EntBrush2D::_OnTransformChanged);
		SetRelativeRotation(Vector3(-90.f, 0.f, 0.f));
	}

	EntBrush2D(const EntBrush2D& other) : EntRenderable(other), _point1(other._point1), _point2(other._point2), level(other.level), _updatingTransform(false)
	{
		onTransformChanged += Callback(this, &EntBrush2D::_OnTransformChanged);
	}

	virtual ~EntBrush2D() {}

	void SetPoint1(const Vector2& position) { _point1 = position; _OnPointChanged(); }
	void SetPoint2(const Vector2& position) { _point2 = position; _OnPointChanged(); }

	void SetMaterial(const SharedPointer<const Material>& material) { _material = material; }
	void SetMaterial(const String& name, const Context& ctx) { _material = ctx.GetPtr<MaterialManager>()->Get(name, ctx); }

	const Vector2& GetPoint1() const { return _point1; }
	const Vector2& GetPoint2() const { return _point2; }
	const SharedPointer<const Material>& GetMaterial() const { return _material; }
	String GetMaterialName(const Context& ctx) const
	{
		if (_material) return ctx.GetPtr<MaterialManager>()->FindNameOf(_material.Ptr());
		return "Unknown";
	}

	virtual const Collider* GetCollider() const override
	{
		static Collider collider(ECollisionChannels::SURFACE, CollisionBox(Box(Vector3(), Vector3(.5f, .5f, .5f))));
		return &collider;
	}

	virtual Bounds GetBounds() const override
	{
		static Bounds bounds(Vector3(.5f, .5f, .5f));
		return bounds;
	}
	
	virtual void Render(RenderQueue&) const override;

	virtual void WriteData(ByteWriter& buffer, NumberedSet<String>& strings, const Context& ctx) const override;
	virtual void ReadData(ByteReader& buffer, const NumberedSet<String>& strings, const Context& ctx) override;

	virtual const PropertyCollection& GetProperties() override;
};
