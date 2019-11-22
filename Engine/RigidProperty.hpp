#pragma once
#include "Property.hpp"

class _DummyProperty : public Property
{
public:
	_DummyProperty(const String& name, PropertyType type, byte flags = 0) : Property(name, type, flags) {}
	virtual ~_DummyProperty() {}
};

class RigidProperty
{
protected:
	void* const _object;
	Property& _baseProperty;

	RigidProperty(void* object, Property& property) : _object(object), _baseProperty(property) {}

public:
	virtual ~RigidProperty() {}

	byte GetFlags() const { return _baseProperty.GetFlags(); }
	PropertyType GetType() const { return _baseProperty.GetType(); }
	const String& GetName() const { return _baseProperty.GetName(); }
	const String& GetDescription() const { return _baseProperty.GetDescription(); }

	const String& GetTypeString() const { return _baseProperty.GetTypeString(); }

	void SetName(const String& name) { return _baseProperty.SetName(name); }
	void SetDescription(const String& description) { return _baseProperty.SetDescription(description); }

	String GetAsString() const { return _baseProperty.GetAsString(_object); }
	void SetAsString(const String& string) const { _baseProperty.SetAsString(_object, string); }
};

template<typename T>
class RigidVariableProperty : public RigidProperty
{
	const VariableProperty<T>& _Property() const { return (const VariableProperty<T>&)_baseProperty; }

protected:
	RigidVariableProperty(void* object, VariableProperty<T>& property) : RigidProperty(object, property) {}

public:
	virtual ~RigidVariableProperty() {}

	virtual T Get() const { return _Property().Get(_object); }
	virtual void Set(const T& value) const { return _Property().Set(_object, value); }
};

template<typename T>
class RigidFptrProperty : public RigidVariableProperty<T>
{
	_DummyProperty _property;

	const bool _isReferenceGetter;
	union
	{
		Getter<const T&>	_getReference;
		Getter<T>			_getObject;
	};

	Setter<T> _set;


public:
	RigidFptrProperty(const Getter<const T&>& getter, const Setter<T>& setter, const Property& property) : 
		RigidVariableProperty<T>(nullptr, _property), 
		_property(property),
		_isReferenceGetter(true),
		_getReference(getter),
		_set(setter)
	{}
	
	RigidFptrProperty(const Getter<T>& getter, const Setter<T>& setter, const Property& property) :
		RigidVariableProperty<T>(nullptr, _property),
		_property(property),
		_isReferenceGetter(false),
		_getObject(getter),
		_set(setter)
	{}

	virtual ~RigidFptrProperty() {};

	virtual T Get() const override { return _isReferenceGetter ? _getReference() : _getObject(); }
	virtual void Set(const T& value) const override { _set(value); }
};

template<typename T>
class RigidReferenceProperty : public RigidVariableProperty<T>
{
	OffsetProperty<T> _property;

public:
	RigidReferenceProperty(const String& name, const T& value, byte flags = 0) : 
		RigidVariableProperty<T>(nullptr, _property), 
		_property(name, (size_t)& value, flags) 
	{}
	
	virtual ~RigidReferenceProperty() {}
};

template<typename T>
class RigidValueProperty : public RigidVariableProperty<T>
{
	OffsetProperty<T> _property;
	T _value;

public:
	RigidValueProperty(const String& name, const T& value, byte flags = 0) : 
		RigidVariableProperty<T>(nullptr, _property), 
		_property(name, (size_t)&_value, flags), 
		_value(value) 
	{}

	virtual ~RigidValueProperty() {}
};
