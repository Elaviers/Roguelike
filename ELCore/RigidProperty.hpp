#pragma once
#include "Property.hpp"

class _DummyProperty : public Property
{
public:
	_DummyProperty(const String& name, byte flags, const char* typeName) : Property(name, flags, typeName) {}
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
	const String& GetName() const { return _baseProperty.GetName(); }
	const String& GetDescription() const { return _baseProperty.GetDescription(); }
	const char* GetTypeName() const { return _baseProperty.GetTypeName(); }

	void SetName(const String& name) { return _baseProperty.SetName(name); }
	void SetDescription(const String& description) { return _baseProperty.SetDescription(description); }

	String GetAsString(const Context& ctx) const { return _baseProperty.GetAsString(_object, ctx); }
	void SetAsString(const String& string, const Context& ctx) const { _baseProperty.SetAsString(_object, string, ctx); }
	String HandleCommand(const Buffer<String>& args, const Context& ctx) const { return _baseProperty.HandleCommand(_object, args, ctx); }
};

template<typename T>
class RigidVariableProperty : public RigidProperty
{
	const VariableProperty<T>& _Property() const { return (const VariableProperty<T>&)_baseProperty; }

protected:
	RigidVariableProperty(void* object, VariableProperty<T>& property) : RigidProperty(object, property) {}

public:
	virtual ~RigidVariableProperty() {}

	virtual T Get(const Context& ctx) const { return _Property().Get(_object, ctx); }
	virtual void Set(const T& value, const Context& ctx) const { return _Property().Set(_object, value, ctx); }
};

template<typename T>
class RigidFptrProperty : public RigidVariableProperty<T>
{
	_DummyProperty _property;

	const bool _isReferenceGetter;
	const bool _isContextualGetter;
	const bool _isContextualSetter;
	union
	{
		Getter<const T&>			_getReference;
		ContextualGetter<const T&>	_getReferenceCtx;
		Getter<T>					_getObject;
		ContextualGetter<T>			_getObjectCtx;
	};

	union
	{
		Setter<T>					_set;
		ContextualSetter<T>			_setCtx;
	};

public:
	//		:/
	RigidFptrProperty(const String& name, const Getter<T>& getter, const Setter<T>& setter, byte flags) : 
		RigidVariableProperty<T>(nullptr, _property), _property(name, flags, typeid(T).name()),
		_isReferenceGetter(false), _isContextualGetter(false), _isContextualSetter(false),
		_getObject(getter), _set(setter)
	{}
	
	RigidFptrProperty(const String& name, const Getter<T>& getter, const ContextualSetter<T>& setter, byte flags) :
		RigidVariableProperty<T>(nullptr, _property), _property(name, flags, typeid(T).name()),
		_isReferenceGetter(false), _isContextualGetter(false), _isContextualSetter(true),
		_getObject(getter), _setCtx(setter)
	{}

	RigidFptrProperty(const String& name, const ContextualGetter<T>& getter, const Setter<T>& setter, byte flags) :
		RigidVariableProperty<T>(nullptr, _property), _property(name, flags, typeid(T).name()),
		_isReferenceGetter(false), _isContextualGetter(true), _isContextualSetter(false),
		_getObjectCtx(getter), _set(setter)
	{}

	RigidFptrProperty(const String& name, const ContextualGetter<T>& getter, const ContextualSetter<T>& setter, byte flags) :
		RigidVariableProperty<T>(nullptr, _property), _property(name, flags, typeid(T).name()),
		_isReferenceGetter(false), _isContextualGetter(true), _isContextualSetter(true),
		_getObjectCtx(getter), _setCtx(setter)
	{}

	RigidFptrProperty(const String& name, const Getter<const T&>& getter, const Setter<T>& setter, byte flags) :
		RigidVariableProperty<T>(nullptr, _property), _property(name, flags, typeid(T).name()),
		_isReferenceGetter(true), _isContextualGetter(false), _isContextualSetter(false),
		_getReference(getter), _set(setter)
	{}

	RigidFptrProperty(const String& name, const Getter<const T&>& getter, const ContextualSetter<T>& setter, byte flags) :
		RigidVariableProperty<T>(nullptr, _property), _property(name, flags, typeid(T).name()),
		_isReferenceGetter(true), _isContextualGetter(false), _isContextualSetter(true),
		_getReference(getter), _setCtx(setter)
	{}

	RigidFptrProperty(const String& name, const ContextualGetter<const T&>& getter, const Setter<T>& setter, byte flags) :
		RigidVariableProperty<T>(nullptr, _property), _property(name, flags, typeid(T).name()),
		_isReferenceGetter(true), _isContextualGetter(true), _isContextualSetter(false),
		_getReferenceCtx(getter), _set(setter)
	{}

	RigidFptrProperty(const String& name, const ContextualGetter<const T&>& getter, const ContextualSetter<T>& setter, byte flags) :
		RigidVariableProperty<T>(nullptr, _property), _property(name, flags, typeid(T).name()),
		_isReferenceGetter(true), _isContextualGetter(true), _isContextualSetter(true),
		_getReferenceCtx(getter), _setCtx(setter)
	{}

	virtual ~RigidFptrProperty() {};

	virtual T Get(const Context& ctx) const override
	{
		if (_isContextualGetter)
			return _isReferenceGetter ? _getReferenceCtx(ctx) : _getObjectCtx(ctx);
		else
			return _isReferenceGetter ? _getReference() : _getObject(); 
	}

	virtual void Set(const T& value, const Context& ctx) const override 
	{ 
		if (_isContextualSetter)
			_setCtx(value, ctx);
		else
			_set(value); 
	}
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
