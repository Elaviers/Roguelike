#pragma once
#include "Entity.hpp"
#include "CollisionBox.hpp"
#include "Engine.hpp"
#include "MaterialManager.hpp"
#include "SharedPointer.hpp"
#include "Vector.hpp"

template <int SIZE>
class EntBrush : public Entity
{
protected:
	Vector<float, SIZE> _point1;
	Vector<float, SIZE> _point2;
	
	SharedPointer<const Material> _material;

	virtual void _OnPointChanged() = 0;

	EntBrush() : Entity(EFlags::SAVEABLE), _material(nullptr) { }
	virtual ~EntBrush() {}

public:
	void SetPoint1(const Vector<float, SIZE> &position) { _point1 = position; _OnPointChanged(); }
	void SetPoint2(const Vector<float, SIZE> &position) { _point2 = position; _OnPointChanged(); }
	void SetMaterial(const SharedPointer<const Material>& material) { _material = material; }
	void SetMaterial(const String &name) { if (Engine::Instance().pMaterialManager) _material = Engine::Instance().pMaterialManager->Get(name); }
	
	const Vector<float, SIZE>& GetPoint1() const { return _point1; }
	const Vector<float, SIZE>& GetPoint2() const { return _point2; }
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
};
