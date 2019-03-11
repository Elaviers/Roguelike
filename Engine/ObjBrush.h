#pragma once
#include "CvarMap.h"
#include "GameObject.h"
#include "ColliderAABB.h"
#include "Engine.h"
#include "MaterialManager.h"
#include "Vector.h"

template <int SIZE>
class ObjBrush : public GameObject
{
protected:
	//todo: add dimensions
	ColliderAABB _collider = ColliderAABB(Vector3(-.5f, -.5f, -.5f), Vector3(.5f, .5f, .5f));
	Bounds _bounds = Bounds(Vector3(-.5f, -.5f, -.5f), Vector3(.5f, .5f, .5f));

	Vector<float, SIZE> _point1;
	Vector<float, SIZE> _point2;
	
	const Material *_material;

	virtual void _UpdateTransform() = 0;

	ObjBrush() : GameObject(FLAG_SAVEABLE), _material(nullptr) { }
	virtual ~ObjBrush() {}

public:
	inline void SetPoint1(const Vector<float, SIZE> &position) { _point1 = position; _UpdateTransform(); }
	inline void SetPoint2(const Vector<float, SIZE> &position) { _point2 = position; _UpdateTransform(); }
	inline void SetMaterial(const Material *material) { _material = material; }
	void SetMaterial(const String &name) { if (Engine::Instance().pMaterialManager)_material = *Engine::Instance().pMaterialManager->Get(name); }
	
	inline const Vector<float, SIZE>& GetPoint1() const { return _point1; }
	inline const Vector<float, SIZE>& GetPoint2() const { return _point2; }
	inline const Material* GetMaterial() const { return _material; }
	String GetMaterialName() const
	{																						//todo: const cast removal
		if (Engine::Instance().pMaterialManager && _material) return Engine::Instance().pMaterialManager->FindNameOf(const_cast<Material*>(_material));
		return "Unknown";
	}

	virtual const Collider* GetCollider() const override { return &_collider; }
	virtual Bounds GetBounds() const override { return _bounds; }
};
