#pragma once
#include "Buffer.h"

class GameObject;

class Collection
{
	Buffer<GameObject*> _objects;
	int _nextSlot;

public:
	Collection() : _nextSlot(-1) {}
	~Collection() {}

	template <typename T>
	T* NewObject()
	{
		GameObject *object = new T();

		if (_nextSlot < 0) _objects.Add(object);
		else
		{
			_objects[_nextSlot] = object;

			_nextSlot = -1;
			for (uint32 i = 0; i < _objects.GetSize(); ++i)
				if (_objects[i] == nullptr)
				{
					_nextSlot = i;
					break;
				}
		}

		return dynamic_cast<T*>(object);
	}

	void DeleteObject(GameObject*);

	void Render() const;

	inline Buffer<GameObject*>& Objects() { return _objects; }
	inline const Buffer<GameObject*>& Objects() const { return _objects; }
};
