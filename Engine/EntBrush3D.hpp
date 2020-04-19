#pragma once
#include "EntRenderable.hpp"
#include "CollisionBox.hpp"

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
		onTransformChanged += FunctionPointer<void>(this, &EntBrush3D::_OnTransformChanged);
	}

	EntBrush3D(const EntBrush3D& other) : EntRenderable(other), _point1(other._point1), _point2(other._point2), _updatingTransform(false)
	{
		onTransformChanged += FunctionPointer<void>(this, &EntBrush3D::_OnTransformChanged);
	}

	virtual ~EntBrush3D() {}

	void SetPoint1(const Vector3& position) { _point1 = position; _OnPointChanged(); }
	void SetPoint2(const Vector3& position) { _point2 = position; _OnPointChanged(); }

	void SetMaterial(const SharedPointer<const Material>& material) { _material = material; }
	void SetMaterial(const String& name) { if (Engine::Instance().pMaterialManager) _material = Engine::Instance().pMaterialManager->Get(name); }

	const Vector3& GetPoint1() const { return _point1; }
	const Vector3& GetPoint2() const { return _point2; }
	const SharedPointer<const Material>& GetMaterial() const { return _material; }
	String GetMaterialName() const
	{
		if (Engine::Instance().pMaterialManager && _material) return Engine::Instance().pMaterialManager->FindNameOf(_material.Ptr());
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

	virtual void Render(ERenderChannels) const override;

	virtual void WriteData(BufferWriter<byte> &buffer, NumberedSet<String> &strings) const override;
	virtual void ReadData(BufferReader<byte> &buffer, const NumberedSet<String> &strings) override;

	virtual const PropertyCollection& GetProperties() override;
};
