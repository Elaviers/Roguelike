#pragma once
#include "RigidProperty.hpp"
#include "Buffer.hpp"

class RigidPropertyCollection
{
	Buffer<RigidProperty*> _cvars;

public:
	RigidPropertyCollection() {}
	~RigidPropertyCollection() 
	{
		for (size_t i = 0; i < _cvars.GetSize(); ++i)
			delete _cvars[i];
	}

	const Buffer<RigidProperty*> GetAll() const { return _cvars; }

	inline void Clear() { _cvars.Clear(); }

	String HandleCommand(const String& command) const;

	inline RigidProperty* Find(const String& name) const
	{
		for (size_t i = 0; i < _cvars.GetSize(); ++i)
			if (_cvars[i]->GetName() == name)
				return _cvars[i];

		return nullptr;
	}

	template <typename T>
	RigidVariableProperty<T>* FindVar(const String &name)
	{
		RigidProperty** property = Find(name);

		if (property && (*property)->GetType() == TypenameToEnum<T>())
			return (RigidVariableProperty<T>*)property->property;

		return nullptr;
	}

	template <typename T>
	bool Get(const String& name, T& out) const
	{
		RigidVariableProperty<T>* property = FindVar<T>(name);
		if (property) out = property->Get();

		return property != nullptr;
	}

	template <typename T>
	void Set(const String& name, const T& value)
	{
		RigidVariableProperty<T>* property = FindVar<T>(name);
		if (property) property->Set(value);
	}

	//////////

	template <typename T>
	inline void CreateVar(const String& name, const T& value, byte flags = 0)
	{
		_cvars.Add(new RigidValueProperty<T>(name, value, flags));
	}

	template <typename T>
	inline void Add(const String &name, T& value, byte flags = 0)
	{
		_cvars.Add(new RigidReferenceProperty<T>(name, value, flags));
	}

	template <typename T>
	inline void Add(const String &name, const Getter<T> &getter, const Setter<T> &setter, byte flags = 0)
	{
		_cvars.Add(new RigidFptrProperty<T>(getter, setter, flags));
	}

	template <typename T>
	inline void Add(const String& name, const Getter<const T&>& getter, const Setter<T>& setter, byte flags = 0)
	{
		_cvars.Add(new RigidFptrProperty<T>(getter, setter, flags));
	}
};
