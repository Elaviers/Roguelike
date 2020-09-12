#pragma once
#include <ELCore/Concepts.hpp>
#include <ELCore/Map.hpp>
#include <ELCore/String.hpp>
#include <ELCore/Utilities.hpp>
#include <ELSys/Debug.hpp>
#include <typeinfo>

class Entity;

template <typename T>
class RegistryNodeBase
{
protected:
	RegistryNodeBase(const String &name) : name(name) {};
public:
	const String name;

	virtual ~RegistryNodeBase() {}

	virtual T* New() = 0;
};

template <typename B, typename D>
requires Concepts::DerivedFrom<D, B>
class RegistryNode : public RegistryNodeBase<B>
{
public:
	RegistryNode(const String &name) : RegistryNodeBase<B>(name) {}
	virtual ~RegistryNode() {}

	virtual B* New() override { return new D(); }
};

template <typename D>
requires Concepts::DerivedFrom<D, Entity>
class RegistryNode<Entity, D> : public RegistryNodeBase<Entity>
{
public:
	RegistryNode(const String& name) : RegistryNodeBase<Entity>(name) {}
	virtual ~RegistryNode() {}

	virtual Entity* New() override { return D::Create(); }
};

template <typename T>
class Registry
{
	Map<byte, RegistryNodeBase<T>*> _registry;

public:
	Registry() {}
	~Registry() {}

	template <typename D>
	requires Concepts::DerivedFrom<D, T>
	void RegisterObjectClass(const String &name)
	{
		byte id = D::TypeID;

		Debug::Assert(id != 0, "An invalid object ID was registered");

		if (_registry.Get(id))
			Debug::Error("Multiple classes can not have the same ID!");
		
		RegistryNode<T, D> *newNode = new RegistryNode<T, D>(name);
		_registry[id] = newNode;
	}

	RegistryNodeBase<T>* GetNode(byte id)
	{
		auto regNode = _registry.Get(id);
		if (regNode)
			return *regNode;

		return nullptr;
	}

	Buffer<Pair<const byte*, RegistryNodeBase<T>* const *>> GetRegisteredTypes() const { return _registry.ToKVBuffer(); }
};
