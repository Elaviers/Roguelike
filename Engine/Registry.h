#pragma once
#include "Debug.h"
#include "Map.h"
#include "GameObject.h"
#include "String.h"
#include "Utilities.h"
#include <typeinfo>

class RegistryNodeBase
{
protected:
	RegistryNodeBase(const String &name) : name(name) {};
public:
	const String name;

	virtual ~RegistryNodeBase() {}

	virtual GameObject* New() = 0;
	virtual GameObject *Object() = 0;

	virtual bool IsType(const GameObject *basePointer) const = 0;
};

template <typename T>
class RegistryNode : public RegistryNodeBase
{
public:
	T object;

	RegistryNode(const String &name) : RegistryNodeBase(name) {}
	virtual ~RegistryNode() {}

	virtual GameObject* New() override { return T::Create(); }
	virtual GameObject* Object() override { return dynamic_cast<GameObject*>(&object); }

	virtual bool IsType(const GameObject *basePointer) const override { return dynamic_cast<const T*>(basePointer) != nullptr; }
};

class Registry
{
	Map<byte, RegistryNodeBase*> _registry;

public:
	Registry() {};
	~Registry() {};

	template <typename T>
	void RegisterObjectClass(byte id, const String &name)
	{
		Debug::Assert(id != 0, "An invalid object ID was registered");

		if (_registry.Find(id))
			Debug::Error("Multiple classes can not have the same ID!");
		
		RegistryNode<T> *newNode = new RegistryNode<T>(name);
		_registry[id] = newNode;
	}

	template<typename T>
	Buffer<RegistryNode<T>*> GetNodesOfType()
	{
		Buffer<RegistryNodeBase*> result;
		auto types = GetRegisteredTypes();
		for (uint32 i = 0; i < types.GetSize(); ++i)
		{
			if ((*types[i].second)->IsType<T>())
				result.Add(*types[i].second);
		}

		return result;
	}

	template<typename T>
	RegistryNode<T>* GetFirstNodeOfType()
	{
		auto types = GetRegisteredTypes();
		for (uint32 i = 0; i < types.GetSize(); ++i)
		{
			auto ptr = dynamic_cast<RegistryNode<T>*>(*types[i].second);
			if (ptr)
				return ptr;
		}

		return nullptr;
	}

	byte GetFirstCompatibleID(const GameObject *obj) const
	{
		//lol

		Buffer<RegistryNodeBase*> allCompatible;

		auto types = GetRegisteredTypes();
		for (uint32 i = 0; i < types.GetSize(); ++i)
		{
			if ((*types[i].second)->IsType(obj))
				allCompatible.Add(*types[i].second);
		}

		if (allCompatible.GetSize() == 0)
			return 0;

		RegistryNodeBase* bestCompatible = allCompatible[0];

		for (uint32 i = 0; i < allCompatible.GetSize(); ++i)
		{
			if (bestCompatible->IsType(allCompatible[i]->Object()))
				bestCompatible = allCompatible[i]; 
		}

		return *_registry.FindFirstKey(bestCompatible);
	}

	inline RegistryNodeBase* GetNode(byte id)
	{
		auto regNode = _registry.Find(id);
		if (regNode)
			return *regNode;

		return nullptr;
	}

	inline Buffer<Pair<const byte*, RegistryNodeBase* const *>> GetRegisteredTypes() const { return _registry.ToBuffer(); }

	void RegisterEngineObjects();
};
