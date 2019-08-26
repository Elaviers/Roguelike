#pragma once
#include "FunctionPointer.hpp"
#include "String.hpp"
#include "Types.hpp"
#include "Vector.hpp"

enum class CvarType
{
	FUNCTION,

	UINT16, UINT32, UINT64,
	INT16, INT32, INT64,
	BYTE,
	FLOAT,

	VECTOR2,
	VECTOR3,
	STRING,

	NONE = 0
};

namespace CvarFlags
{
	enum CvarFlag
	{
		READONLY = 0x01,
		MODEL = 0x02,
		MATERIAL = 0x04,
		CLASSID = 0x08,
		DIRECTION = 0x10,
		CHECKBOX = 0x20
	};
}

template <typename T> inline CvarType TypenameToEnum() { return CvarType::NONE; }
template<> inline CvarType TypenameToEnum<CommandPtr>() { return CvarType::FUNCTION; }
template<> inline CvarType TypenameToEnum<float>() { return CvarType::FLOAT; }
template<> inline CvarType TypenameToEnum<byte>() { return CvarType::BYTE; }
template<> inline CvarType TypenameToEnum<uint16>() { return CvarType::UINT16; }
template<> inline CvarType TypenameToEnum<uint32>() { return CvarType::UINT32; }
template<> inline CvarType TypenameToEnum<uint64>() { return CvarType::UINT64; }
template<> inline CvarType TypenameToEnum<int16>() { return CvarType::INT16; }
template<> inline CvarType TypenameToEnum<int32>() { return CvarType::INT32; }
template<> inline CvarType TypenameToEnum<int64>() { return CvarType::INT64; }
template<> inline CvarType TypenameToEnum<Vector2>() { return CvarType::VECTOR2; }
template<> inline CvarType TypenameToEnum<Vector3>() { return CvarType::VECTOR3; }
template<> inline CvarType TypenameToEnum<String>() { return CvarType::STRING; }

class Cvar
{
	CvarType _type;
	byte _flags;

public:
	int index;

	Cvar(CvarType type, byte flags) : _type(type), _flags(flags), index(-1) {}
	Cvar(const Cvar &other) : _type(other._type), _flags(other._flags), index(other.index) {}
	virtual ~Cvar() {}

	virtual size_t SizeOf() = 0;

	void SetByString(const String &value);
	String GetAsString() const;

	void TransferTo(Cvar &other);

	inline byte GetFlags() { return _flags; }
	inline CvarType GetType() { return _type; }
};

template <typename T>
class TypedCvar : public Cvar
{
public:
	TypedCvar(byte flags) : Cvar(TypenameToEnum<T>(), flags) {}
	virtual ~TypedCvar() {}

	virtual T Get() const = 0;
	virtual void Set(const T&) = 0;
};

template <>
class TypedCvar<CommandPtr> : public Cvar
{
public:
	TypedCvar(byte flags) : Cvar(TypenameToEnum<CommandPtr>(), flags) {}
	virtual ~TypedCvar() {}

	virtual CommandPtr Get() const = 0;
	virtual void Set(const CommandPtr&) = 0;

	inline void Call(const Buffer<String>& args) const { Get()(args); }
};

template <typename T>
class TypedCallbackCvar : public TypedCvar<T>
{
	bool _isReferenceGetter;
	union
	{
		FunctionPointer<const T&> _getByReference;
		Getter<T> _getByObject;
	};
	Setter<T> _set;

public:
	TypedCallbackCvar(const Getter<T> &getter, const Setter<T> &setter, byte flags = 0) : TypedCvar<T>(flags), _isReferenceGetter(false), _getByObject(getter), _set(setter) {}
	TypedCallbackCvar(const Getter<const T&>& getter, const Setter<T>& setter, byte flags = 0) : TypedCvar<T>(flags), _isReferenceGetter(true), _getByReference(getter), _set(setter) {}
	virtual ~TypedCallbackCvar() {}

	virtual T Get() const override { return _isReferenceGetter ? _getByReference() : _getByObject(); }
	virtual void Set(const T &value) override { _set(value); }

	virtual size_t SizeOf() { return sizeof(*this); }
};

template <typename T>
class TypedReferenceCvar : public TypedCvar<T>
{
	T& _value;

public:
	TypedReferenceCvar(T &value, byte flags = 0) : TypedCvar<T>(flags), _value(value) {}
	virtual ~TypedReferenceCvar() {}

	virtual T Get() const override { return _value; }
	virtual void Set(const T &value) override { _value = value; }

	virtual size_t SizeOf() { return sizeof(*this); }
};

template <typename T>
class TypedValueCvar : public TypedCvar<T>
{
	T _value;

public:
	TypedValueCvar(const T& value, byte flags = 0) : TypedCvar<T>(flags), _value(value) {}
	virtual ~TypedValueCvar() {}

	virtual T Get() const override { return _value; }
	virtual void Set(const T& value) override { _value = value; }

	virtual size_t SizeOf() { return sizeof(*this); }
};
