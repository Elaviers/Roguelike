#pragma once
#include "Entity.hpp"

inline Entity* _GetNextEntity(const Entity* entity)
{
	if (entity)
	{
		//If we are a parent return first child
		if (entity->Children().GetSize() > 0)
			return entity->Children()[0];

		const Entity* parent = entity->GetParent();
		const Entity* child = entity;

		while (parent)
		{
			//if there is an element in parent after child return it
			for (size_t i = 0; i < parent->Children().GetSize() - 1; ++i)
				if (parent->Children()[i] == child)
					return parent->Children()[i + 1];

			//otherwise use the parent as the child of the parent's parent
			child = parent;
			parent = parent->GetParent();
		}
	}

	return nullptr;
}

class EntityIterator
{
	Entity* _entity;
	Entity* _startingPoint;

	EntityIterator(Entity* entity, Entity* startingPoint) : _entity(entity), _startingPoint(startingPoint) {}

public:
	EntityIterator(Entity* entity) : _entity(entity), _startingPoint(entity) {}
	~EntityIterator() {}

	bool IsValid() const { return _entity != nullptr; }

	Entity& operator*() { return *_entity; }
	Entity* operator->() { return _entity; }
	const Entity& operator*() const { return *_entity; }
	const Entity* operator->() const { return _entity; }

	EntityIterator& operator++() { *this = Next(); return *this; }

	EntityIterator Next() const 
	{
		Entity* next = _GetNextEntity(_entity);
		if (next && next != _startingPoint) return EntityIterator(next, _startingPoint);
		return EntityIterator(nullptr);
	}
};

class ConstEntityIterator
{
	const Entity* _entity;
	const Entity* _startingPoint;

	ConstEntityIterator(const Entity* entity, const Entity* startingPoint) : _entity(entity), _startingPoint(startingPoint) {}

public:
	ConstEntityIterator(const Entity* entity) : _entity(entity), _startingPoint(entity) {}
	~ConstEntityIterator() {}

	bool IsValid() const { return _entity != nullptr; }

	const Entity& operator*() const { return *_entity; }
	const Entity* operator->() const { return _entity; }

	ConstEntityIterator& operator++() { *this = Next(); return *this; }

	ConstEntityIterator Next() const
	{
		const Entity* next = _GetNextEntity(_entity);
		if (next && next != _startingPoint) return ConstEntityIterator(next, _startingPoint);
		return ConstEntityIterator(nullptr);
	}
};

