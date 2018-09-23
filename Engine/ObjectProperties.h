#pragma once
#include "Error.h"
#include "Map.h"
#include "Pair.h"
#include "String.h"
#include "Vector.h"
#include "Utilities.h"

enum class PropertyType
{
	UINT16, UINT32, UINT64,
	INT16, INT32, INT64,
	BYTE,
	FLOAT,

	VECTOR2,
	VECTOR3,
	STRING,

	NONE
};

namespace PropertyFlags
{
	enum PropertyFlag
	{
		MODEL = 0x01, 
		MATERIAL = 0x02
	};
}

inline PropertyType TypenameToEnum(const float&) { return PropertyType::FLOAT; }
inline PropertyType TypenameToEnum(const byte&) { return PropertyType::BYTE; }
inline PropertyType TypenameToEnum(const int16&) { return PropertyType::INT16; }
inline PropertyType TypenameToEnum(const int32&) { return PropertyType::INT32; }
inline PropertyType TypenameToEnum(const int64&) { return PropertyType::INT64; }
inline PropertyType TypenameToEnum(const uint16) { return PropertyType::UINT16; }
inline PropertyType TypenameToEnum(const uint32&) { return PropertyType::UINT32; }
inline PropertyType TypenameToEnum(const uint64&) { return PropertyType::UINT64; }
inline PropertyType TypenameToEnum(const Vector2&) { return PropertyType::VECTOR2; }
inline PropertyType TypenameToEnum(const Vector3&) { return PropertyType::VECTOR3; }
inline PropertyType TypenameToEnum(const String&) { return PropertyType::STRING; }
//Ideally this would be done with templates but it was being smelly so function overloading it is

template <typename T> inline PropertyType TypenameToEnum() { return TypenameToEnum(T()); }
//Yeah, this really stinks

class Property
{
	PropertyType _type;
	byte _flags;

public:
	int index;

	Property(PropertyType type, byte flags) : _type(type), _flags(flags) {}
	Property(const Property&);
	virtual ~Property() {}

	virtual uint32 SizeOf() = 0;

	void SetByString(const String &value);
	String GetAsString() const;

	inline byte GetFlags() { return _flags; }
	inline PropertyType GetType() { return _type; }
};

template <typename T>
class PropertyBase : public Property
{
public:
	PropertyBase(byte flags) : Property(TypenameToEnum<T>(), flags) {}
	virtual ~PropertyBase() {}

	virtual T Get() const = 0;
	virtual void Set(const T&) = 0;

	
};

template <typename T>
class Property_Raw : public PropertyBase<T>
{
	void **_base;
	void *_offset;

public:
	Property_Raw(void **base, T *pointer, byte flags = 0) : PropertyBase<T>(flags), _base(base), _offset((void*)((byte*)pointer - (byte*)*_base)) {}
	
	virtual T Get() const override
	{
		if (*_base)	return *((T*)((byte*)*_base + (int)_offset));
		else		return *(T*)_offset;
	}

	virtual void Set(const T &value) override
	{
		if (*_base) *((T*)((byte*)*_base + (int)_offset)) = value;
		else		*(T*)_offset = value;
	}

	virtual uint32 SizeOf() { return sizeof(*this); }
};

template <typename Base, typename T>
class Property_FPointer : public PropertyBase<T>
{
	void **_base;
	uint16 _offset;

	T(Base::*_getter)() const;
	void (Base::*_setter)(const T&);
public:
	
	Property_FPointer(void **base, Base *pointer, T(Base::*getter)() const, void(Base::*setter)(const T&), byte flags = 0) : PropertyBase<T>(flags), _base(base),
		_getter(getter), _setter(setter), _offset((uint16)((uint64)pointer - (uint64)*_base)) {}

	virtual T Get() const override
	{
		if (_getter) return ((Base*)((byte*)*_base + _offset)->*_getter)();
		else return T();
	}

	virtual void Set(const T &value) override
	{
		if (_setter)
			((Base*)((byte*)*_base + _offset)->*_setter)(value);
	}

	virtual uint32 SizeOf() { return sizeof(*this); }
};

class ObjectProperties
{
	void *_base;
	Map<String, Property*> _properties;

	int _count;

public:
	ObjectProperties() : _base(nullptr), _count(0) {}
	~ObjectProperties() {}

	template <typename Base>
	inline void SetBase(Base object) { _base = object; }

	inline Property** FindRaw(const char *name) { return _properties.Find(name); }

	template <typename Type>
	PropertyBase<Type>* Find(const char *name)
	{
		Property** property = FindRaw(name);

		if (property && (*property)->GetType() == TypenameToEnum<Type>())
			return (PropertyBase<Type>*)property->property;

		return nullptr;
	}

	template <typename Type>
	void Add(const char *name, Type& value, byte flags = 0)
	{
		Property_Raw<Type> *property = new Property_Raw<Type>(&_base, &value, flags);
		property->index = _count++;
		_properties.Set(name, property);
	}

	template <typename Base, typename Type>
	void Add(const char *name, Base *member, Type(Base::*getter)() const, void(Base::*setter)(const Type&), byte flags = 0)
	{
		Property_FPointer<Base, Type> *property = new Property_FPointer<Base, Type>(&_base, member, getter, setter, flags);
		property->index = _count++;
		_properties.Set(name, property);
	}

	inline void Clear() { _properties.Clear(); _count = 0; }

	

	//Caution: uses a static member
	Buffer<Pair<String, Property*>> GetAll()
	{
		static Buffer<Pair<String, Property*>> propertyArray; //Yuck
		static int nodeCount;

		nodeCount = 0;
		
		_properties.ForEach([](const String&, Property*&) { ++nodeCount; });

		propertyArray.SetSize(nodeCount);

		_properties.ForEach([](const String &name, Property* &property)
		{
			propertyArray[property->index].first = name;
			propertyArray[property->index].second = property;
		});

		return propertyArray;
	}

	inline ObjectProperties& operator=(const ObjectProperties &other)
	{
		Clear();

		_base = other._base;
		_count = other._count;

		_properties = other._properties;
		
		_properties.ForEach(
		[](const String &key, Property* &property) {
			Property* old = property;
			property = (Property*)new byte[old->SizeOf()];

			Utilities::CopyBytes(old, property, old->SizeOf());
		});

		return *this;
	}
};
