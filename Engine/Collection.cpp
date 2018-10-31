#include "Collection.h"
#include "GameObject.h"
#include "Utilities.h"

void Collection::InsertObject(GameObject *object)
{
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
}

void Collection::DeleteObject(GameObject *object)
{
	for (uint32 i = 0; i < _objects.GetSize(); ++i)
		if (_objects[i] == object)
		{
			_objects[i] = nullptr;
			break;
		}

	delete object;
}

void Collection::Render() const
{
	for (uint32 i = 0; i < _objects.GetSize(); ++i)
		if (_objects[i])
			_objects[i]->Render();
}

Buffer<RaycastResult> Collection::Raycast(const Ray &ray)
{
	RaycastResult result;
	Buffer<RaycastResult> results;

	for (uint32 i = 0; i < _objects.GetSize(); ++i)
		if (_objects[i])
			if (_objects[i]->Raycast(ray, result))
			{
				result.object = _objects[i];

				uint32 index = 0;
				while (index < results.GetSize())
					if (results[index].entryTime > result.entryTime)
						break;
					else index++;

				results.Insert(result, index);
			}
	
	return results;
}

Buffer<GameObject*> Collection::FindOverlaps(const Collider &collider, const Transform &transform)
{
	Buffer<GameObject*> results;

	for (uint32 i = 0; i < _objects.GetSize(); ++i)
		if (_objects[i])
			if (_objects[i]->Overlaps(collider, transform))
				results.Add(_objects[i]);

	return results;
}
