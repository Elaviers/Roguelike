#pragma once
#include "Utilities.hpp"

class _PtrData
{
public:
	void* obj;
	void* ptr;

	_PtrData(void* obj, void* ptr) : obj(obj), ptr(ptr) {}

	bool operator==(const _PtrData& other) const { return ptr == other.ptr && obj == other.obj; }
	bool operator!=(const _PtrData& other) const { return ptr != other.ptr || obj != other.obj; }
};

template<typename T, typename RETURNTYPE, typename ...Args>
class MemberFunctionPointer
{
private:
	union
	{
		RETURNTYPE(T::* _function)(Args...);
		RETURNTYPE(T::* _constFunction)(Args...) const;

		void* _voidPtr;
	};

	bool _isConstFunction;

public:
	MemberFunctionPointer() : _isConstFunction(false), _function(nullptr) {}
	MemberFunctionPointer(RETURNTYPE(T::* function)(Args...)) :			_isConstFunction(false), _function(function)		{}
	MemberFunctionPointer(RETURNTYPE(T::* function)(Args...) const) :	_isConstFunction(true), _constFunction(function)	{}

	RETURNTYPE Call(T& obj, Args ...args) const
	{
		if (_isConstFunction)
			return (obj.*_constFunction)(args...);
		
		return (obj.*_function)(args...);
	}

	RETURNTYPE Call(const T& obj, Args ...args) const
	{
		//todo: not this
		return (obj.*_constFunction)(args...);
	}

	void* GetRawPointer() const { return _voidPtr; }

	bool operator==(const MemberFunctionPointer& other) const { return _voidPtr == other._voidPtr && _isConstFunction == other._isConstFunction; }
	bool operator!=(const MemberFunctionPointer& other) const { return _voidPtr != other._voidPtr || _isConstFunction != other._isConstFunction; }
};

/*
	FunctionPointer

	A pointer to any function, as either a static function or a member function of a specific class instance
*/

template <typename RETURNTYPE, typename ...Args>
class FunctionPointer
{
	class FunctionCaller
	{
	protected:
		virtual _PtrData _GetPtrData() const = 0;

	public:
		virtual ~FunctionCaller() {}

		virtual RETURNTYPE Call(Args...) const = 0;
		virtual bool IsCallable() const = 0;

		void TryCall(Args... args) const	{ if (IsCallable()) Call(args...); }

		bool operator==(const FunctionCaller& other) const { return _GetPtrData() == other._GetPtrData(); }
		bool operator!=(const FunctionCaller& other) const { return _GetPtrData() != other._GetPtrData(); }
	};

	class FunctionCallerStatic : public FunctionCaller
	{
		RETURNTYPE(*_function)(Args...);

	protected:
		virtual _PtrData _GetPtrData() const override { return _PtrData(nullptr, _function); }

	public:
		FunctionCallerStatic(RETURNTYPE(*function)(Args...)) : _function(function) {}
		virtual ~FunctionCallerStatic() {}

		virtual RETURNTYPE Call(Args... args) const override	{ return _function(args...); }
		virtual bool IsCallable() const							{ return _function != nullptr; }
	};

	template<typename T>
	class FunctionCallerMember : public FunctionCaller
	{
		T* _object;
		MemberFunctionPointer<T, RETURNTYPE, Args...> _fp;

	protected:
		virtual _PtrData _GetPtrData() const override { return _PtrData(_object, _fp.GetRawPointer()); }
		
	public:
		FunctionCallerMember() : _object(nullptr), _fp() {}
		FunctionCallerMember(T* object, RETURNTYPE(T::* function)(Args...)) : _object(object), _fp(function) {}
		FunctionCallerMember(T* object, RETURNTYPE(T::* function)(Args...) const) : _object(object), _fp(function) {}

		virtual ~FunctionCallerMember() {}

		virtual RETURNTYPE Call(Args... args) const override	{ return _fp.Call(*_object, args...); }
		virtual bool IsCallable() const							{ return _object != nullptr; }
	};

	/////
	union Union
	{
		FunctionCallerStatic				fptrStatic;

		//_PtrData is a placeholder...
		FunctionCallerMember<_PtrData>		fptrMember;

		Union(const FunctionCallerStatic& _static) : fptrStatic(_static) {}

		template<typename T>
		Union(const FunctionCallerMember<T>& _member) : fptrMember()
		{
			//This is disgostan
			Utilities::CopyBytes(&_member, this, sizeof(_member));
		}

		~Union() {}
	} _u;

	FunctionCaller& _Fptr()				{ return reinterpret_cast<FunctionCaller&>(_u); }
	const FunctionCaller& _Fptr() const	{ return reinterpret_cast<const FunctionCaller&>(_u); }

public:
	FunctionPointer(RETURNTYPE(*function)(Args...) = nullptr) : _u(FunctionCallerStatic(function)) {}

	template<typename T>
	FunctionPointer(T* object, RETURNTYPE(T::* function)(Args...)) : _u(FunctionCallerMember<T>(object, function)) {}

	template<typename T>
	FunctionPointer(T* object, RETURNTYPE(T::* function)(Args...) const) : _u(FunctionCallerMember<T>(object, function)) {}

	FunctionPointer(const FunctionPointer& other) : _u(nullptr) { operator=(other); }

	FunctionPointer(FunctionPointer&& other) { operator=(other); }

	~FunctionPointer() { }

	FunctionPointer& operator=(const FunctionPointer& other)
	{
		Utilities::CopyBytes(&other._u, &_u, sizeof(_u));
		return *this;
	}

	FunctionPointer& operator=(FunctionPointer&& other)
	{
		Utilities::CopyBytes(&other._u, &_u, sizeof(_u));
		other._Fptr() = FunctionCallerStatic(nullptr);
		return *this;
	}

	RETURNTYPE operator()(Args ...args)			{ return _Fptr().Call(args...); }
	RETURNTYPE operator()(Args ...args) const	{ return _Fptr().Call(args...); }
	bool IsCallable() const						{ return _Fptr().IsCallable(); }
	void TryCall(Args ...args)					{ _Fptr().TryCall(args...); }
	void TryCall(Args ...args) const			{ _Fptr().TryCall(args...); }

	bool operator==(const FunctionPointer& other) const { return (FunctionCaller&)_u == (FunctionCaller&)other._u; }
	bool operator!=(const FunctionPointer& other) const { return (FunctionCaller&)_u != (FunctionCaller&)other._u; }
};

typedef FunctionPointer<void> Callback;

template<typename T>
using Getter = FunctionPointer<T>;

template<typename T, typename RETURNTYPE>
using MemberGetter = MemberFunctionPointer<T, RETURNTYPE>;

template<typename T>
using Setter = FunctionPointer<void, const T&>;

template<typename T, typename V>
using MemberSetter = MemberFunctionPointer<T, void, const V&>;

using NewHandler = FunctionPointer<byte*, size_t>;

using DeleteHandler = FunctionPointer<void, byte*>;

class Context;
typedef FunctionPointer<void, const Buffer<String>&, const Context&> CommandPtr;

template<typename T>
using MemberCommandPtr = MemberFunctionPointer<T, void, const Buffer<String>&, const Context&>;

template<typename T>
using ContextualGetter = FunctionPointer<T, const Context&>;

template<typename T, typename RETURNTYPE>
using ContextualMemberGetter = MemberFunctionPointer<T, RETURNTYPE, const Context&>;

template<typename T>
using ContextualSetter = FunctionPointer<void, const T&, const Context&>;

template<typename T, typename V>
using ContextualMemberSetter = MemberFunctionPointer<T, void, const V&, const Context&>;
