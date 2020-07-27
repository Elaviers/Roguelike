#pragma once
#include "Entity.hpp"
#include <ELCore/Map.hpp>
#include <ELCore/String.hpp>
#include <ELCore/Utilities.hpp>
#include <ELSys/Debug.hpp>
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
	Map<byte, RegistryNodeBase*> _registry;

public:
	Registry() {}
	~Registry() {}

	template <typename T>
	void RegisterObjectClass(const String &name)
	{
		byte id = T::TypeID;

		Debug::Assert(id != 0, "An invalid object ID was registered");

		if (_registry.Get(id))
			Debug::Error("Multiple classes can not have the same ID!");
		
		RegistryNode<T> *newNode = new RegistryNode<T>(name);
		_registry[id] = newNode;
	}

	RegistryNodeBase* GetNode(byte id)
	{
		auto regNode = _registry.Get(id);
		if (regNode)
			return *regNode;

		return nullptr;
	}

	Buffer<Pair<const byte*, RegistryNodeBase* const *>> GetRegisteredTypes() const { return _registry.ToKVBuffer(); }

	void RegisterEngineObjects();
};
