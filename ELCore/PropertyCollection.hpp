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

	void Clear() { _cvars.Clear(); }

	String HandleCommand(void* obj, const Buffer<String>& tokens, const Context&) const;
	String HandleCommand(void* obj, const String& command, const Context& ctx) const { return HandleCommand(obj, command.Split(" "), ctx); }

	void Transfer(const void* from, void* to, const Context& ctx) const
	{
		for (size_t i = 0; i < _cvars.GetSize(); ++i)
			_cvars[i]->TransferTo(from, to, ctx);
	}

	Property* Find(const String& name) const
	{
		for (size_t i = 0; i < _cvars.GetSize(); ++i)
			if (_cvars[i]->GetName().Equals(name, true))
				return _cvars[i];

		return nullptr;
	}
	
	template <typename V>
	VariableProperty<V>* FindVar(const String& name) const
	{
		return dynamic_cast<VariableProperty<V>*>(Find(name));
	}

	template <typename V>
	bool Get(const void* obj, const String& name, V& out, const Context& ctx) const
	{
		VariableProperty<V>* property = FindVar<V>(name);
		if (property) out = property->Get(obj, ctx);

		return property != nullptr;
	}

	template <typename V>
	void Set(void* obj, const String& name, const V& value, const Context& ctx)
	{
		VariableProperty<V>* property = FindVar<V>(name);
		if (property) property->Set(obj, value, ctx);
	}

	//////////

	template <typename V>
	void Add(const String& name, size_t offset, byte flags = 0)
	{
		_cvars.Add(new OffsetProperty<V>(name, offset, flags));
	}
	
	template <typename T, typename V> void Add(const String& name, const MemberGetter<T, V>& getter,					const MemberSetter<T, V>& setter,			size_t offset = 0, byte flags = 0)	{ _cvars.Add(new FptrProperty<T, V>(name, getter, setter, offset, flags)); }
	template <typename T, typename V> void Add(const String& name, const MemberGetter<T, V>& getter,					const ContextualMemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0)	{ _cvars.Add(new FptrProperty<T, V>(name, getter, setter, offset, flags)); }
	template <typename T, typename V> void Add(const String& name, const ContextualMemberGetter<T, V>& getter,			const MemberSetter<T, V>& setter,			size_t offset = 0, byte flags = 0)	{ _cvars.Add(new FptrProperty<T, V>(name, getter, setter, offset, flags)); }
	template <typename T, typename V> void Add(const String& name, const ContextualMemberGetter<T, V>& getter,			const ContextualMemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0)	{ _cvars.Add(new FptrProperty<T, V>(name, getter, setter, offset, flags)); }
	template <typename T, typename V> void Add(const String& name, const MemberGetter<T, const V&>& getter,				const MemberSetter<T, V>& setter,			size_t offset = 0, byte flags = 0)	{ _cvars.Add(new FptrProperty<T, V>(name, getter, setter, offset, flags)); }
	template <typename T, typename V> void Add(const String& name, const MemberGetter<T, const V&>& getter,				const ContextualMemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0)	{ _cvars.Add(new FptrProperty<T, V>(name, getter, setter, offset, flags)); }
	template <typename T, typename V> void Add(const String& name, const ContextualMemberGetter<T, const V&>& getter,	const MemberSetter<T, V>& setter,			size_t offset = 0, byte flags = 0)	{ _cvars.Add(new FptrProperty<T, V>(name, getter, setter, offset, flags)); }
	template <typename T, typename V> void Add(const String& name, const ContextualMemberGetter<T, const V&>& getter,	const ContextualMemberSetter<T, V>& setter, size_t offset = 0, byte flags = 0)	{ _cvars.Add(new FptrProperty<T, V>(name, getter, setter, offset, flags)); }

	template <typename T>
	void AddCommand(const String& name, const MemberCommandPtr<T>& function, size_t offset = 0, byte flags = 0)
	{
		_cvars.Add(new FunctionProperty<T>(name, function, offset));
	}
};
