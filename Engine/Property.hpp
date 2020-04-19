#pragma once
#include "FunctionPointer.hpp"
#include "String.hpp"
#include "Types.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

enum class EPropertyType
{
	FUNCTION,

	BOOL,
	UINT16, UINT32, UINT64,
	INT16, INT32, INT64,
	BYTE,
	FLOAT,

	VECTOR2,
	VECTOR3,
	VECTOR4,
	STRING,

	NONE = 0
};

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

template <typename T> inline EPropertyType TypenameToEnum() { return EPropertyType::NONE; }
template<> inline EPropertyType TypenameToEnum<CommandPtr>() { return EPropertyType::FUNCTION; }
template<> inline EPropertyType TypenameToEnum<bool>() { return EPropertyType::BOOL; }
template<> inline EPropertyType TypenameToEnum<float>() { return EPropertyType::FLOAT; }
template<> inline EPropertyType TypenameToEnum<byte>() { return EPropertyType::BYTE; }
template<> inline EPropertyType TypenameToEnum<uint16>() { return EPropertyType::UINT16; }
template<> inline EPropertyType TypenameToEnum<uint32>() { return EPropertyType::UINT32; }
template<> inline EPropertyType TypenameToEnum<uint64>() { return EPropertyType::UINT64; }
template<> inline EPropertyType TypenameToEnum<int16>() { return EPropertyType::INT16; }
template<> inline EPropertyType TypenameToEnum<int32>() { return EPropertyType::INT32; }
template<> inline EPropertyType TypenameToEnum<int64>() { return EPropertyType::INT64; }
template<> inline EPropertyType TypenameToEnum<Vector2>() { return EPropertyType::VECTOR2; }
template<> inline EPropertyType TypenameToEnum<Vector3>() { return EPropertyType::VECTOR3; }
template<> inline EPropertyType TypenameToEnum<Vector4>() { return EPropertyType::VECTOR4; }
template<> inline EPropertyType TypenameToEnum<String>() { return EPropertyType::STRING; }

class Property
{
	String _name;
	String _description;
	
	EPropertyType _type;
	byte _flags;

protected:
	Property(const String& name, EPropertyType type, byte flags) : _name(name), _type(type), _flags(flags) {}

public:
	virtual ~Property() {}

	byte GetFlags() const { return _flags; }
	EPropertyType GetType() const { return _type; }
	const String& GetName() const { return _name; }
	const String& GetDescription() const { return _description; }

	const String& GetTypeString() const;

	void SetName(const String& name) { _name = name; }
	void SetDescription(const String& description) { _description = description; }

	String GetAsString(const void* obj) const;
	void SetAsString(void* obj, const String& value) const;

	void TransferTo(const void* from, void* to) const;
};

class FunctionPropertyBase : public Property
{
protected:
	FunctionPropertyBase(const String& name) : Property(name, EPropertyType::FUNCTION, 0) {}
public:
	virtual ~FunctionPropertyBase() {}
	
	virtual void Call(void* obj, const Buffer<String>& tokens) const = 0;
};

template <typename T>
class FunctionProperty : public FunctionPropertyBase
{
	size_t _offset;

	MemberCommandPtr<T> _fptr;

	T& GetSubClass(void* obj) const { return *(T*)((byte*)obj + _offset); }
public:
	FunctionProperty(const String& name, const MemberCommandPtr<T>& function, size_t offset = 0) : FunctionPropertyBase(name), _offset(offset), _fptr(function) {}
	virtual ~FunctionProperty() {}

	virtual void Call(void* obj, const Buffer<String>& tokens) const { _fptr.Call(GetSubClass(obj), tokens); }
};

template <typename V>
class VariableProperty : public Property
{
protected:
	VariableProperty(const String& name, byte flags = 0) : Property(name, TypenameToEnum<V>(), flags) {}

public:
	virtual ~VariableProperty() {}

	virtual V Get(const void* obj) const = 0;
	virtual void Set(void* obj, const V& value) const = 0;
};

template<typename T, typename V>
class FptrProperty : public VariableProperty<V>
{
	size_t _offset;

	const bool _isReferenceGetter;
	union
	{
		MemberGetter<T, const V&>			_getReference;
		MemberGetter<T, V>				_getObject;
	};

	MemberSetter<T, V>					_set;

	T& GetSubClass(void* obj) const { return *(T*)((byte*)obj + _offset); }
	const T& GetSubClass(const void* obj) const { return *(const T*)((byte*)obj + _offset); }

public:
	FptrProperty(const String& name, const MemberGetter<T, V>& getter, const MemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0) :
		VariableProperty<V>(name, flags),
		_isReferenceGetter(false),
		_getObject(getter),
		_offset(offset),
		_set(setter)
	{}

	FptrProperty(const String& name, const MemberGetter<T, const V&>& getter, const MemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0) :
		VariableProperty<V>(name, flags),
		_isReferenceGetter(true),
		_getReference(getter),
		_offset(offset),
		_set(setter)
	{}

	virtual ~FptrProperty() {}

	virtual V Get(const void* obj) const override { return _isReferenceGetter ? _getReference.Call(GetSubClass(obj)) : _getObject.Call(GetSubClass(obj)); }
	virtual void Set(void* obj, const V& value) const override { _set.Call(GetSubClass(obj), value); }
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

	virtual V Get(const void* obj) const override
	{
		return GetRef(obj);
	}

	virtual void Set(void* obj, const V& value) const override
	{
		V* ptr = (V*)((byte*)obj + _offset);
		*ptr = value;
	}
};
