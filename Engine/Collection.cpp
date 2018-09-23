#include "Collection.h"
#include "GameObject.h"

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
