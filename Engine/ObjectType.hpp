#pragma once
#include "EObjectID.hpp"
#include "Register.hpp"
#include <ELCore/String.hpp>
#include <ELCore/Function.hpp>

class World;
class WorldObject;

struct ObjectType
{
private:
	EObjectID _id;
	String _name;
	MemberFunction<World, WorldObject*> _create;

public:
	ObjectType(EObjectID id, const String& name, const MemberFunction<World, WorldObject*>& create) :
		_id(id), _name(name), _create(create) {}

	EObjectID GetID() const { return _id; }
	const String& GetName() const { return _name; }

	WorldObject* New(World& world) const { return _create.Call(world); }
};

class ObjectRegister : public Register<ObjectType, EObjectID>
{
protected:
	virtual EObjectID _GetTypeID(const ObjectType& type) const override { return type.GetID(); }
	virtual String _GetTypeName(const ObjectType& type) const override { return type.GetName(); }
};
