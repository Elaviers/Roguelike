#pragma once
#include "GameObject.h"
#include "ColliderAABB.h"
#include "Engine.h"
#include "ObjectProperties.h"

template <int SIZE>
class Brush : public GameObject
{
protected:
	ColliderAABB _collider;

	Vector<float, SIZE> _point1;
	Vector<float, SIZE> _point2;
	
	const Material *_material;

	virtual void _UpdateTransform() = 0;

public:
	Brush() : _material(nullptr) { }
	virtual ~Brush() {}

	inline void SetPoint1(const Vector<float, SIZE> &position) { _point1 = position; _UpdateTransform(); }
	inline void SetPoint2(const Vector<float, SIZE> &position) { _point2 = position; _UpdateTransform(); }
	inline void SetMaterial(const Material *material) { _material = material; }
	void SetMaterial(const String &name) { if (Engine::materialManager)_material = Engine::materialManager->GetMaterial(name); }
	
	inline const Vector<float, SIZE>& GetPoint1() const { return _point1; }
	inline const Vector<float, SIZE>& GetPoint2() const { return _point2; }
	inline const Material* GetMaterial() const { return _material; }
	String GetMaterialName() const
	{																						//todo: const cast removal
		if (Engine::materialManager && _material) return Engine::materialManager->FindNameOf(const_cast<Material*>(_material));
		return "Unknown";
	}

	//Collision
	virtual const Collider* GetCollider() const override { return &_collider; }
	virtual bool Raycast(const Ray &ray, RaycastResult &result) const override { return _collider.IntersectsRay(ray, result); }
	virtual bool Overlaps(const Collider &collider, const Transform &transform) const override { return _collider.Overlaps(collider, transform); }

	virtual Bounds GetBounds() const override { return Bounds(_collider.min, _collider.max); }

	GAMEOBJ_STD_OVERRIDES;
};
