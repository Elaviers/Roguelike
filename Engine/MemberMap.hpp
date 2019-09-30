#pragma once
#include "FunctionPointer.hpp"
#include "Hashmap.hpp"
#include <initializer_list>

/*
	

*/

template<typename T>
class MemberMap
{
	class MemberAccessor
	{
	protected:
		MemberAccessor() {}

	public:
		String name;
		String uiName;

		virtual ~MemberAccessor() {}
	};

	class MemberCommand : public MemberAccessor
	{
		void (T::*_fptr)(const Buffer<String>&);

	public:
		MemberCommand(void (T::fptr)(const Buffer<String>&)) : _fptr(fptr) { static_assert(fptr != nullptr, "Don't create member commands with nullptrs!") }
		virtual ~MemberCommand() {}

		inline void Call(const T& obj, const Buffer<String>& tokens)
		{
			obj->*_fptr(tokens);
		}
	};

	struct MemberVariableBase : public MemberAccessor
	{
	protected:
		size_t _offset;

		MemberVariableBase(size_t offset) : _offset(offset) {}
	public:
		virtual ~MemberVariableBase() {}
	};

	template<typename V>
	struct MemberVariable : public MemberVariableBase
	{
		inline V* _Ptr(const T& obj) { return (V*)((byte*)(obj)+_offset); }
	public:
		MemberVariable(size_t offset) : MemberVariableBase(offset) {}
		virtual ~MemberVariable() {}

		inline const V& Get(const T& obj)				{ return *_Ptr(obj); }
		inline void Set(const T& obj, const V& value)	{ *_Ptr(obj) = value; }
	};

	Hashmap<String, MemberCommand> _commands;
	Hashmap<String, MemberVariableBase> _variables;

public:
	MemberMap(std::initializer_list<MemberCommand> commands, std::initializer_list<MemberVariableBase> variables) 
	{
		for (const MemberCommand* it = commands.begin(); it < commands.end(); ++it)
			_commands.Set(it->name, *it);

		for (const MemberVariableBase* it = variables.begin(); it < variables.end(); ++it)
			_variables.Set(it->name, *it);
	}

	~MemberMap() {}

	inline void Call(const T& obj, const String& name, const Buffer<String>& tokens)
	{
		MemberCommand* memberCommand = _commands.Get(name);

		if (memberCommand)
			memberCommand->Call(obj, tokens);
	}

	
	template<typename V>
	inline void Set(const T& obj, const String& name, const V& value)
	{
		auto memberVariable = dynamic_cast<MemberVariable<V>*>(_variables.Get(name));

		if (memberVariable)
			memberVariable->Set(obj, value);
	}

	template<typename V>
	inline const V* Get(const T& obj, const String& name)
	{
		auto memberVariable = dynamic_cast<MemberVariable<V>*>(_variables.Get(name));

		if (memberVariable)
			return &memberVariable->Get(obj);

		return nullptr;
	}
};
