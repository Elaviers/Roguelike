#pragma once
#include "EntRenderable.hpp"
#include <ELCore/Context.hpp>
#include <ELPhys/CollisionBox.hpp>

class EntBrush3D : public EntRenderable
{
protected:
	Vector3 _point1;
	Vector3 _point2;

	void _OnTransformChanged();
	void _OnPointChanged();

	bool _updatingTransform;

public:
	Entity_FUNCS(EntBrush3D, EEntityID::BRUSH);

	EntBrush3D() : _updatingTransform(false)
	{ 
		onTransformChanged += Callback(*this, &EntBrush3D::_OnTransformChanged);
	}

	EntBrush3D(const EntBrush3D& other) : EntRenderable(other), _point1(other._point1), _point2(other._point2), _updatingTransform(false)
	{
		onTransformChanged += Callback(*this, &EntBrush3D::_OnTransformChanged);
	}

	virtual ~EntBrush3D() {}

	void SetPoint1(const Vector3& position) { _point1 = position; _OnPointChanged(); }
	void SetPoint2(const Vector3& position) { _point2 = position; _OnPointChanged(); }

	const Vector3& GetPoint1() const { return _point1; }
	const Vector3& GetPoint2() const { return _point2; }

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

	virtual void WriteData(ByteWriter &buffer, NumberedSet<String> &strings, const Context& ctx) const override;
	virtual void ReadData(ByteReader &buffer, const NumberedSet<String> &strings, const Context& ctx) override;

	virtual const PropertyCollection& GetProperties() override;
};
