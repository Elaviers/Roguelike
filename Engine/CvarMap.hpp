#pragma once
#include "Cvar.hpp"
#include "Map.hpp"

class CvarMap
{
	Map<String, Cvar*> _cvars;

	int _count;

public:
	CvarMap() : _count(0) {}
	~CvarMap() {}

	inline Cvar* FindRaw(const String &name) 
	{ 
		Cvar** found = _cvars.Find(name); 
		
		if (found) return *found;
		return nullptr;
	}

	template <typename Type>
	TypedCvar<Type>* Find(const String &name)
	{
		Cvar** cvar = FindRaw(name);

		if (cvar && (*cvar)->GetType() == TypenameToEnum<Type>())
			return (TypedCvar<Type>*)cvar->cvar;

		return nullptr;
	}

	/*
		Creates a new variable and initialises with existing value
	*/
	template <typename Type>
	void CreateVar(const String& name, const Type& value, byte flags = 0)
	{
		TypedValueCvar<Type>* cvar = new TypedValueCvar<Type>(value, flags);

		cvar->index = _count++;
		_cvars.Set(name, cvar);
	}

	/*
		Creates a new variable that refers to an existing variable
	*/
	template <typename Type>
	void Add(const String &name, Type& value, byte flags = 0)
	{
		TypedReferenceCvar<Type> *cvar = new TypedReferenceCvar<Type>(value, flags);
		
		cvar->index = _count++;
		_cvars.Set(name, cvar);
	}

	/*
		Creates a new variable that is accessed via function pointers
	*/
	template <typename T>
	void Add(const String &name, const Getter<T> &getter, const Setter<T> &setter, byte flags = 0)
	{
		TypedCallbackCvar<T> *cvar = new TypedCallbackCvar<T>(getter, setter, flags);

		cvar->index = _count++;
		_cvars.Set(name, cvar);
	}

	/*
		Creates a new variable that is accessed via function pointers
	*/
	template <typename T>
	void Add(const String& name, const Getter<const T&>& getter, const Setter<T>& setter, byte flags = 0)
	{
		TypedCallbackCvar<T>* cvar = new TypedCallbackCvar<T>(getter, setter, flags);

		cvar->index = _count++;
		_cvars.Set(name, cvar);
	}

	template <typename T>
	void Set(const String &name, const T &value)
	{
		auto cvar = _cvars.Find(name);
		if (cvar)
		{
			if ((*cvar)->GetType() == TypenameToEnum<T>())
				(*reinterpret_cast<TypedCvar<T>*>(cvar))->Set(value);
		}
	}

	inline void Clear() { _cvars.Clear(); _count = 0; }

	inline Buffer<Pair<const String*, Cvar* const *>> GetAll() { return _cvars.ToBuffer();}

	CvarMap& operator=(const CvarMap &other)
	{
		Clear();

		_count = other._count;

		_cvars = other._cvars;
		
		_cvars.ForEach(
		[](const String &key, Cvar* &property) {
				Cvar* old = property;
			uint32 size = (uint32)old->SizeOf();
			property = (Cvar*)new byte[size];
			Utilities::CopyBytes(old, property, size);
		});

		return *this;
	}

	void TransferValuesTo(CvarMap &other)
	{
		auto buffer = _cvars.ToBuffer();

		for (uint32 i = 0; i < buffer.GetSize(); ++i)
		{
			Cvar *key = *buffer[i].second;
			Cvar *otherKey = other._cvars[*buffer[i].first];
			if (otherKey && key->GetType() == otherKey->GetType())
			{
				key->TransferTo(*otherKey);
			}
		}
	}

	String HandleCommand(const String &command);
};
