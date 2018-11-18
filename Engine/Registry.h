#pragma once
#include "Map.h"
#include "GameObject.h"
#include "String.h"
#include "Utilities.h"

class RegistryNodeBase
{
public:
	virtual ~RegistryNodeBase() {}

	virtual GameObject* New() = 0;
	virtual GameObject *Object() = 0;

	virtual bool IsType(GameObject *basePointer) = 0;
};

template <typename T>
class RegistryNode : public RegistryNodeBase
{
	T object;

public:
	RegistryNode() {}
	virtual ~RegistryNode() {}

	virtual GameObject* New() override { return (GameObject*)(new T()); }
	virtual GameObject* Object() override { return &object; }

	virtual bool IsType(GameObject *basePointer) override { return dynamic_cast<T*>(basePointer) != nullptr; }
};

class Registry
{
	Map<String, RegistryNodeBase*> _registry;

public:
	Registry() {};
	~Registry() {};

	template <typename T>
	void RegisterObjectClass(const String &name)
	{
		RegistryNode<T> *newNode = new RegistryNode<T>();
		_registry[name] = newNode;
	}

	inline GameObject* NewObjectFromClassName(const String &className) const
	{
		auto regNode = _registry.Find(className);
		if (regNode)
			return (*regNode)->New();

		return nullptr;
	}

	inline GameObject* GetObjectInstanceFromClassName(const String &className)
	{
		auto regNode = _registry.Find(className);
		if (regNode)
			return (*regNode)->Object();
	}

	inline Buffer<Pair<const String*, RegistryNodeBase* const *>> GetRegisteredTypes() const { return _registry.ToBuffer(); }

	void RegisterEngineObjects();
};
