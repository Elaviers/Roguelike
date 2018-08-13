#pragma once
#include "Map.h"
#include "String.h"
#include "Vector.h"

enum class PropertyType
{
	UINT16, UINT32, UINT64,
	INT16, INT32, INT64,
	BYTE,
	FLOAT,

	VECTOR3
};

inline PropertyType TypenameToEnum(const float&) { return PropertyType::FLOAT; }
inline PropertyType TypenameToEnum(const byte&) { return PropertyType::BYTE; }
inline PropertyType TypenameToEnum(const int16&) { return PropertyType::INT16; }
inline PropertyType TypenameToEnum(const int32&) { return PropertyType::INT32; }
inline PropertyType TypenameToEnum(const int64&) { return PropertyType::INT64; }
inline PropertyType TypenameToEnum(const uint16) { return PropertyType::UINT16; }
inline PropertyType TypenameToEnum(const uint32&) { return PropertyType::UINT32; }
inline PropertyType TypenameToEnum(const uint64&) { return PropertyType::UINT64; }
inline PropertyType TypenameToEnum(const Vector3&) { return PropertyType::VECTOR3; }
//Ideally this would be done with templates but it was being smelly so function overloading it is

template <typename T> inline PropertyType TypenameToEnum() { return TypenameToEnum(T()); }
//Yeah, this really stinks

template <typename T>
class PropertyBase
{
public:
	virtual T Get() const = 0;
	virtual void Set(const T&) = 0;
};

template <typename Base, typename T>
class Property : public PropertyBase<T>
{
	void **_base;

	bool _isRaw = false;	//If true, pointer is used

	uint16 _offset;

	struct
	{
		T(Base::*_getter)() const;
		void (Base::*_setter)(const T&);
	};

public:
	Property(void **base, T *pointer) : _base(base), _isRaw(true), _offset((uint16)((byte*)pointer - (byte*)*_base)) {}
	Property(void **base, Base *pointer, T(Base::*getter)() const, void(Base::*setter)(const T&)) : _base(base), _isRaw(false),
		_getter(getter), _setter(setter), _offset((uint16)((uint64)pointer - (uint64)*_base)) {}

	virtual T Get() const override
	{
		if (_isRaw) return *(T*)((byte*)*_base + _offset);
		else if (_getter) return ((Base*)((byte*)*_base + _offset)->*_getter)();
		else return T();
	}

	virtual void Set(const T &value) override
	{
		if (_isRaw)	*(T*)((byte*)*_base + _offset) = value;
		else if (_setter) ((Base*)((byte*)*_base + _offset)->*_setter)(value);
	}
};

struct PropertyPointer
{
	PropertyType type;
	void *property; //ALWAYS points to a PropertyBase

	PropertyPointer(PropertyType type, void *propertyPtr) : type(type), property(propertyPtr) {}
};

class ObjectProperties
{
	void *_base;
	Map<String, PropertyPointer> _properties;

public:
	ObjectProperties() {}
	ObjectProperties(void *base) : _base(base) {}
	~ObjectProperties() {}

	template <typename Type>
	PropertyBase<Type>* Find(const char *name)
	{
		PropertyPointer *property = _properties.Find(name);

		if (property && property->type == TypenameToEnum<Type>())
			return (PropertyBase<Type>*)property->property;

		return nullptr;
	}

	template <typename Type>
	void Add(const char *name, const Type& value)
	{
		Property<void, Type> *property = new Property<void, Type>(&_base, &value - _base);

		_properties.Set(name, PropertyPointer(TypenameToEnum<Type>(), property));

	}

	template <typename Base, typename Type>
	void Add(const char *name, Base *member, Type(Base::*getter)() const, void(Base::*setter)(const Type&))
	{
		Property<Base, Type> *property = new Property<Base, Type>(&_base, member, getter, setter);

		_properties.Set(name, PropertyPointer(TypenameToEnum<Type>(), property));
	}
};
