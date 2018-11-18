#pragma once
#include "Error.h"
#include "Pair.h"
#include "Map.h"
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
		READONLY = 0x01,
		MODEL = 0x02,
		MATERIAL = 0x04,
		CLASSNAME = 0x08
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

protected:
	void **_base;

public:
	int index;

	Property(void **base, PropertyType type, byte flags) : _base(base), _type(type), _flags(flags), index(-1) {}
	Property(const Property&);
	virtual ~Property() {}

	virtual size_t SizeOf() = 0;

	void SetByString(const String &value);
	String GetAsString() const;

	void Transfer(void *otherBase);

	inline byte GetFlags() { return _flags; }
	inline PropertyType GetType() { return _type; }
};

template <typename T>
class PropertyBase : public Property
{
public:
	PropertyBase(void **base, byte flags) :  Property(base, TypenameToEnum<T>(), flags) {}
	virtual ~PropertyBase() {}

	virtual T Get(void *base) const = 0;
	virtual void Set(const T&, void *base) = 0;

	inline T Get() const { return Get(*_base); }
	inline void Set(const T &value) { Set(value, *_base); }
};

template <typename T>
class Property_Raw : public PropertyBase<T>
{
	bool _isOffset;

	union
	{
		T *_ptr;
		uint16 _offset;
	};

public:
	Property_Raw(void **base, uint16 offset, byte flags = 0) : PropertyBase<T>(base, flags), _isOffset(true), _offset(offset) {}
	Property_Raw(void **base, T *ptr, byte flags = 0) : PropertyBase<T>(base, flags), _isOffset(false), _ptr(ptr) {}

	virtual T Get(void *base) const override
	{
		if (_isOffset)	return *((T*)((byte*)base + _offset));
		else			return *_ptr;
	}

	virtual void Set(const T &value, void *base) override
	{
		if (_isOffset) *((T*)((byte*)base + _offset)) = value;
		else		*_ptr = value;
	}

	virtual size_t SizeOf() { return sizeof(*this); }
};

template <typename Base, typename T>
class Property_FPointer : public PropertyBase<T>
{
	uint16 _offset;

	T (Base::*_getter)() const;
	void (Base::*_setter)(const T&);
public:
	
	Property_FPointer(void **base, uint16 offset, T(Base::*getter)() const, void(Base::*setter)(const T&), byte flags = 0) : PropertyBase<T>(base, flags), _getter(getter), _setter(setter), _offset(offset) {}

	virtual T Get(void *base) const override
	{
		if (_getter) return ((Base*)((byte*)base + _offset)->*_getter)();
		else return T();
	}

	virtual void Set(const T &value, void *base) override
	{
		if (_setter)
			((Base*)((byte*)base + _offset)->*_setter)(value);
	}

	virtual size_t SizeOf() { return sizeof(*this); }
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
	inline void SetBase(Base &object) { _base = &object; }

	inline Property** FindRaw(const String &name) { return _properties.Find(name); }

	template <typename Type>
	PropertyBase<Type>* Find(const String &name)
	{
		Property** property = FindRaw(name);

		if (property && (*property)->GetType() == TypenameToEnum<Type>())
			return (PropertyBase<Type>*)property->property;

		return nullptr;
	}

	template <typename Type>
	void Add(const String &name, Type& value, byte flags = 0)
	{
		Property_Raw<Type> *property;

		if (_base) property = new Property_Raw<Type>(&_base, (uint16)((uint64)&value - (uint64)_base), flags);
		else property = new Property_Raw<Type>(&_base, &value, flags);
		
		property->index = _count++;
		_properties.Set(name, property);
	}

	template <typename Base, typename Type>
	void Add(const String &name, Base *member, Type(Base::*getter)() const, void(Base::*setter)(const Type&), byte flags = 0)
	{
		Property_FPointer<Base, Type> *property = new Property_FPointer<Base, Type>(&_base, (uint16)((uint64)member - (uint64)_base), getter, setter, flags);
		property->index = _count++;
		_properties.Set(name, property);
	}

	template <typename T>
	void Set(const String &name, const T &value)
	{
		auto property = _properties.Find(name);
		if (property)
		{
			if ((*property)->GetType() == TypenameToEnum<T>())
				(*reinterpret_cast<PropertyBase<T>*>(property))->Set(value);
		}
	}

	inline void Clear() { _properties.Clear(); _count = 0; }

	inline Buffer<Pair<const String*, Property* const *>> GetAll() { return _properties.ToBuffer();}

	void Apply(void *object)
	{
		auto props = GetAll();

		for (uint32 i = 0; i < props.GetSize(); ++i)
			(*props[i].second)->Transfer(object);
	}

	ObjectProperties& operator=(const ObjectProperties &other)
	{
		Clear();

		_base = other._base;
		_count = other._count;

		_properties = other._properties;
		
		_properties.ForEach(
		[](const String &key, Property* &property) {
			Property* old = property;
			uint32 size = (uint32)old->SizeOf();
			property = (Property*)new byte[size];
			Utilities::CopyBytes(old, property, size);
		});

		return *this;
	}
};
