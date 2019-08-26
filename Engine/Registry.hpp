#pragma once
#include "Debug.hpp"
#include "Map.hpp"
#include "GameObject.hpp"
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

	virtual GameObject* New() = 0;
};

template <typename T>
class RegistryNode : public RegistryNodeBase
{
public:
	RegistryNode(const String &name) : RegistryNodeBase(name) {}
	virtual ~RegistryNode() {}

	virtual GameObject* New() override { return T::Create(); }
};

class Registry
{
	Map<byte, RegistryNodeBase*> _registry;

public:
	Registry() {}
	~Registry() {}

	template <typename T>
	void RegisterObjectClass(byte id, const String &name)
	{
		Debug::Assert(id != 0, "An invalid object ID was registered");

		if (_registry.Get(id))
			Debug::Error("Multiple classes can not have the same ID!");
		
		RegistryNode<T> *newNode = new RegistryNode<T>(name);
		_registry[id] = newNode;
	}

	inline RegistryNodeBase* GetNode(byte id)
	{
		auto regNode = _registry.Get(id);
		if (regNode)
			return *regNode;

		return nullptr;
	}

	inline Buffer<Pair<const byte*, RegistryNodeBase* const *>> GetRegisteredTypes() const { return _registry.ToKVBuffer(); }

	void RegisterEngineObjects();
};
