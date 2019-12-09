#pragma once
#include "Debug.hpp"
#include "Map.hpp"
#include "Entity.hpp"
#include "String.hpp"
#include "Utilities.hpp"
#include <typeinfo>

class RegistryNodeBase
{
protected:
	RegistryNodeBase(const String &name) : name(name) {};
public:
	const String name;

	virtual ~RegistryNodeBase() {}

	virtual Entity* New() = 0;
};

template <typename T>
class RegistryNode : public RegistryNodeBase
{
public:
	RegistryNode(const String &name) : RegistryNodeBase(name) {}
	virtual ~RegistryNode() {}

	virtual Entity* New() override { return T::Create(); }
};

class Registry
{
	Map<EntityID, RegistryNodeBase*> _registry;

public:
	Registry() {}
	~Registry() {}

	template <typename T>
	void RegisterObjectClass(const String &name)
	{
		byte id = T::TypeID;

		Debug::Assert(id != EntityID::NONE, "An invalid object ID was registered");

		if (_registry.Get(id))
			Debug::Error("Multiple classes can not have the same ID!");
		
		RegistryNode<T> *newNode = new RegistryNode<T>(name);
		_registry[id] = newNode;
	}

	RegistryNodeBase* GetNode(EntityID id)
	{
		auto regNode = _registry.Get(id);
		if (regNode)
			return *regNode;

		return nullptr;
	}

	Buffer<Pair<const EntityID*, RegistryNodeBase* const *>> GetRegisteredTypes() const { return _registry.ToKVBuffer(); }

	void RegisterEngineObjects();
};
