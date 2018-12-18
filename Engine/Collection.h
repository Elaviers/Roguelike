#pragma once
#include "Buffer.h"
#include "Camera.h"
#include "Collider.h"
#include "RaycastResult.h"

class GameObject;
struct Ray;

class Collection
{
	Buffer<GameObject*> _objects;
	int _nextSlot;

public:
	Collection() : _nextSlot(-1) {}
	~Collection() {}

	void InsertObject(GameObject*);

	template <typename T>
	inline T* NewObject()
	{
		GameObject *object = new T();
		InsertObject(object);
		return dynamic_cast<T*>(object);
	}

	void Clear();

	void RemoveObject(GameObject*);

	void Render(const Camera &camera) const;

	inline Buffer<GameObject*>& Objects() { return _objects; }
	inline const Buffer<GameObject*>& Objects() const { return _objects; }

	Buffer<RaycastResult> Raycast(const Ray&);

	Buffer<GameObject*> FindOverlaps(const Collider&, const Transform& = Transform());
};
