#pragma once
#include "Property.hpp"
#include "Buffer.hpp"

class PropertyCollection
{
	Buffer<Property*> _cvars;

public:
	PropertyCollection() {}
	~PropertyCollection() 
	{
		for (size_t i = 0; i < _cvars.GetSize(); ++i)
			delete _cvars[i];
	}

	const Buffer<Property*>& GetAll() const { return _cvars; }

	inline void Clear() { _cvars.Clear(); }

	String HandleCommand(void* obj, const Buffer<String>& tokens) const;
	inline String HandleCommand(void* obj, const String& command) const { return HandleCommand(obj, command.Split(" ")); }

	void Transfer(const void* from, void* to) const
	{
		for (size_t i = 0; i < _cvars.GetSize(); ++i)
			_cvars[i]->TransferTo(from, to);
	}

	inline Property* Find(const String& name) const
	{
		for (size_t i = 0; i < _cvars.GetSize(); ++i)
			if (_cvars[i]->GetName().Equals(name, true))
				return _cvars[i];

		return nullptr;
	}
	
	template <typename V>
	VariableProperty<V>* FindVar(const String& name) const
	{
		Property* property = Find(name);
		
		if (property && property->GetType() == TypenameToEnum<V>())
			return dynamic_cast<VariableProperty<V>*>(property);

		return nullptr;
	}

	FunctionPropertyBase* FindFunc(const String& name) const
	{
		Property* property = Find(name);

		if (property && property->GetType() == PropertyType::FUNCTION)
			return dynamic_cast<FunctionPropertyBase*>(property);

		return nullptr;
	}

	template <typename V>
	bool Get(const void* obj, const String& name, V& out) const
	{
		VariableProperty<V>* property = FindVar<V>(name);
		if (property) out = property->Get(obj);

		return property != nullptr;
	}

	template <typename V>
	void Set(void* obj, const String& name, const V& value)
	{
		VariableProperty<V>* property = FindVar<V>(name);
		if (property) property->Set(obj, value);
	}

	//////////

	template <typename V>
	inline void Add(const String& name, size_t offset, byte flags = 0)
	{
		_cvars.Add(new OffsetProperty<V>(name, offset, flags));
	}
	
	template <typename T, typename V>
	inline void Add(const String& name, const MemberGetter<T, V>& getter, const MemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0)
	{
		_cvars.Add(new FptrProperty<T, V>(name, getter, setter, offset, flags));
	}

	template <typename T, typename V>
	inline void Add(const String& name, const MemberGetter<T, const V&>& getter, const MemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0)
	{
		_cvars.Add(new FptrProperty<T, V>(name, getter, setter, offset, flags));
	}

	template <typename T>
	inline void AddCommand(const String& name, const MemberCommandPtr<T>& function, size_t offset = 0, byte flags = 0)
	{
		_cvars.Add(new FunctionProperty<T>(name, function, offset));
	}
};
