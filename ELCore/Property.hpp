#pragma once
#include "Buffer.hpp"
#include "FunctionPointer.hpp"
#include "String.hpp"
#include "Types.hpp"
#include <typeinfo>

namespace PropertyFlags
{
	enum ELevelMessage
	{
		READONLY = 0x01,
		MODEL = 0x02,
		MATERIAL = 0x04,
		CLASSID = 0x08,
		DIRECTION = 0x10
	};
}

class Property
{
	String _name;
	String _description;
	
	byte _flags;

	const char* _typeName;

protected:
	Property(const String& name, byte flags, const char* typeName) : _name(name), _flags(flags), _typeName(typeName) {}

public:
	virtual ~Property() {}

	byte GetFlags() const { return _flags; }
	const String& GetName() const { return _name; }
	const String& GetDescription() const { return _description; }
	const char* GetTypeName() const { return _typeName; }

	void SetName(const String& name) { _name = name; }
	void SetDescription(const String& description) { _description = description; }

	virtual String GetAsString(const void* obj, const Context& ctx) const { return ""; }
	virtual void SetAsString(void* obj, const String& value, const Context& ctx) const {}
	virtual void TransferTo(const void* from, void* to, const Context& ctx) const {}
	virtual String HandleCommand(void* obj, const Buffer<String>& args, const Context& ctx) const 
	{ 
		SetAsString(obj, args[0], ctx);
		return GetName() + " : " + GetAsString(obj, ctx) + "\n";
	}
};

template <typename T>
class FunctionProperty : public Property
{
	size_t _offset;

	MemberCommandPtr<T> _fptr;

	T& GetSubClass(void* obj) const { return *(T*)((byte*)obj + _offset); }
public:
	FunctionProperty(const String& name, const MemberCommandPtr<T>& function, size_t offset = 0) : Property(name, 0, "function"), _offset(offset), _fptr(function) {}
	virtual ~FunctionProperty() {}

	virtual String HandleCommand(void* obj, const Buffer<String>& tokens, const Context& ctx) const override { _fptr.Call(GetSubClass(obj), tokens, ctx); return ""; }
};

template <typename V>
class VariableProperty : public Property
{
protected:
	VariableProperty(const String& name, byte flags = 0) : Property(name, flags, typeid(V).name()) {}

public:
	virtual ~VariableProperty() { }

	virtual V Get(const void* obj, const Context& ctx) const = 0;
	virtual void Set(void* obj, const V& value, const Context& ctx) const = 0;

	virtual String GetAsString(const void* obj, const Context& ctx) const override { return String::From(Get(obj, ctx)); }
	virtual void SetAsString(void* obj, const String& value, const Context& ctx) const override { Set(obj, value.To<V>(), ctx); }
	virtual void TransferTo(const void* from, void* to, const Context& ctx) const override { Set(to, Get(from, ctx), ctx); }
};

template <>
class VariableProperty<CommandPtr> : public Property
{
protected:
	VariableProperty(const String& name, byte flags = 0) : Property(name, flags, typeid(CommandPtr).name()) {}

public:
	virtual ~VariableProperty() { }

	virtual CommandPtr Get(const void* obj, const Context& ctx) const = 0;
	virtual void Set(void* obj, const CommandPtr& value, const Context& ctx) const = 0;

	virtual String GetAsString(const void* obj, const Context& ctx) const override { return "(function)"; }
	virtual String HandleCommand(void* obj, const Buffer<String>& args, const Context& ctx) const
	{
		Get(obj, ctx)(args, ctx);
		return "";
	}
};

template<typename T, typename V>
class FptrProperty : public VariableProperty<V>
{
	size_t _offset;

	const bool _isReferenceGetter;
	const bool _isContextualGetter;
	const bool _isContextualSetter;
	union
	{
		MemberGetter<T, const V&>			_getReference;
		ContextualMemberGetter<T, const V&> _getReferenceCtx;
		MemberGetter<T, V>					_getObject;
		ContextualMemberGetter<T, V>		_getObjectCtx;
	};

	union
	{
		MemberSetter<T, V>					_set;
		ContextualMemberSetter<T, V>		_setCtx;
	};

	T& GetSubClass(void* obj) const { return *(T*)((byte*)obj + _offset); }
	const T& GetSubClass(const void* obj) const { return *(const T*)((byte*)obj + _offset); }

public:
	//YUCK! WTF am I doing?

	FptrProperty(const String& name, const MemberGetter<T, V>& getter, const MemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0) :
		VariableProperty<V>(name, flags), _offset(offset),
		_isReferenceGetter(false), _isContextualGetter(false), _isContextualSetter(false),
		_getObject(getter), _set(setter)
	{}

	FptrProperty(const String& name, const MemberGetter<T, V>& getter, const ContextualMemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0) :
		VariableProperty<V>(name, flags), _offset(offset),
		_isReferenceGetter(false), _isContextualGetter(false), _isContextualSetter(true),
		_getObject(getter), _setCtx(setter)
	{}

	FptrProperty(const String& name, const ContextualMemberGetter<T, V>& getter, const MemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0) :
		VariableProperty<V>(name, flags), _offset(offset),
		_isReferenceGetter(false), _isContextualGetter(true), _isContextualSetter(false),
		_getObjectCtx(getter), _set(setter)
	{}

	FptrProperty(const String& name, const ContextualMemberGetter<T, V>& getter, const ContextualMemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0) :
		VariableProperty<V>(name, flags), _offset(offset),
		_isReferenceGetter(false), _isContextualGetter(true), _isContextualSetter(true),
		_getObjectCtx(getter), _setCtx(setter)
	{}

	FptrProperty(const String& name, const MemberGetter<T, const V&>& getter, const MemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0) :
		VariableProperty<V>(name, flags), _offset(offset),
		_isReferenceGetter(true), _isContextualGetter(false), _isContextualSetter(false),
		_getReference(getter), _set(setter)
	{}

	FptrProperty(const String& name, const MemberGetter<T, const V&>& getter, const ContextualMemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0) :
		VariableProperty<V>(name, flags), _offset(offset),
		_isReferenceGetter(true), _isContextualGetter(false), _isContextualSetter(true),
		_getReferenceCtx(getter), _setCtx(setter)
	{}

	FptrProperty(const String& name, const ContextualMemberGetter<T, const V&>& getter, const MemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0) :
		VariableProperty<V>(name, flags), _offset(offset),
		_isReferenceGetter(true), _isContextualGetter(true), _isContextualSetter(false),
		_getReference(getter),	_set(setter)
	{}

	FptrProperty(const String& name, const ContextualMemberGetter<T, const V&>& getter, const ContextualMemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0) :
		VariableProperty<V>(name, flags), _offset(offset),
		_isReferenceGetter(true), _isContextualGetter(true), _isContextualSetter(true),
		_getReferenceCtx(getter), _setCtx(setter)
	{}

	virtual ~FptrProperty() {}

	virtual V Get(const void* obj, const Context& ctx) const override 
	{
		if (_isContextualGetter)
			return _isReferenceGetter ? _getReferenceCtx.Call(GetSubClass(obj), ctx) : _getObjectCtx.Call(GetSubClass(obj), ctx);
		else
			return _isReferenceGetter ? _getReference.Call(GetSubClass(obj)) : _getObject.Call(GetSubClass(obj)); 
	}
	
	virtual void Set(void* obj, const V& value, const Context& ctx) const override 
	{
		if (_isContextualGetter)
			_setCtx.Call(GetSubClass(obj), value, ctx);
		else
			_set.Call(GetSubClass(obj), value); 
	}
};

template<typename V>
class OffsetProperty : public VariableProperty<V>
{
	size_t _offset;

public:
	OffsetProperty(const String& name, size_t offset, byte flags = 0) : VariableProperty<V>(name, flags), _offset(offset) {}

	virtual ~OffsetProperty() {}

	const V& GetRef(const void* obj) const
	{
		const V* ptr = (const V*)((const byte*)obj + _offset);
		return *ptr;
	}

	virtual V Get(const void* obj, const Context& ctx) const override
	{
		return GetRef(obj);
	}

	virtual void Set(void* obj, const V& value, const Context& ctx) const override
	{
		V* ptr = (V*)((byte*)obj + _offset);
		*ptr = value;
	}
};
