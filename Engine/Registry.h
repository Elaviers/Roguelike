#pragma once
#include "Buffer.h"
#include "GameObject.h"
#include "ObjectProperties.h"
#include "String.h"
#include "Utilities.h"

class RegistryNodeBase
{
	String _name;

public:
	ObjectProperties properties;

	RegistryNodeBase(const char *name) : _name(name) {}
	virtual ~RegistryNodeBase() {}

	inline String GetName() const { return _name; };

	virtual GameObject* Create() = 0;
	
	virtual bool IsType(GameObject *basePointer) = 0;
};

template <typename T>
class RegistryNode : public RegistryNodeBase
{
public:
	RegistryNode(const char *name) : RegistryNodeBase(name) {}
	virtual ~RegistryNode() {}

	virtual GameObject* Create() override { return (GameObject*)(new T()); }

	virtual bool IsType(GameObject *basePointer) override { return dynamic_cast<T*>(basePointer) != nullptr; }
};

class Registry
{
	Buffer<RegistryNodeBase*> _registry;

public:
	Registry() {};
	~Registry() {};

	template <typename T>
	void RegisterObjectClass(const char *name)
	{
		RegistryNode<T> *newNode = new RegistryNode<T>(name);
		_registry.Add(newNode);
	}

	void RegisterEngineObjects();
};
