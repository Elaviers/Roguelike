#pragma once
#include "Debug.h"
#include "Utilities.h"

template <typename RETURNTYPE, typename ...Args>
class FunctionPointer
{
	enum Exception
	{
		EXCEPTION_NULL_STATIC_CALLBACK
	};

	class BaseCallback
	{
	public:
		virtual ~BaseCallback() {}

		virtual RETURNTYPE Call(Args...) = 0;
		virtual RETURNTYPE Call(Args...) const = 0;
		virtual size_t SizeOf() const = 0;
	};

	class StaticCallback : public BaseCallback
	{
		RETURNTYPE (*_function)(Args...);

	public:
		StaticCallback(RETURNTYPE (*function)(Args...)) : _function(function) {}
		virtual ~StaticCallback() {}

		virtual RETURNTYPE Call(Args ...args) override 
		{ 
			if (_function == nullptr)
			{
				Debug::FatalError("Something called a StaticCallback with a nullptr");
				throw EXCEPTION_NULL_STATIC_CALLBACK;
			}

			return _function(args...); 
		}

		virtual RETURNTYPE Call(Args ...args) const override
		{
			if (_function == nullptr)
			{
				Debug::FatalError("Something called a StaticCallback with a nullptr");
				throw EXCEPTION_NULL_STATIC_CALLBACK;
			}

			return _function(args...);
		}

		virtual size_t SizeOf() const override { return sizeof(*this); }
	};

	template<typename T>
	class MemberCallback : public BaseCallback
	{
		T *_object;

		union
		{
			RETURNTYPE(T::*_function)(Args...);
			RETURNTYPE(T::*_constFunction)(Args...) const;
		};

		bool _isConstFunction;

	public:
		MemberCallback(T *object, RETURNTYPE(T::*function)(Args...)) : _object(object), _function(function), _isConstFunction(false) {}
		MemberCallback(T *object, RETURNTYPE(T::*function)(Args...) const) : _object(object), _constFunction(function), _isConstFunction(true) {}

		virtual ~MemberCallback() {}

		virtual RETURNTYPE Call(Args... args) override { return _isConstFunction ? (_object->*_constFunction)(args...) : (_object->*_function)(args...); }
		virtual RETURNTYPE Call(Args... args) const override { return _isConstFunction ? (_object->*_constFunction)(args...) : (_object->*_function)(args...); }
		virtual size_t SizeOf() const override { return sizeof(*this); }
	};

	/////

	BaseCallback *_cb;

public:
	FunctionPointer(RETURNTYPE (*function)(Args...) = nullptr) : _cb(nullptr)			{ if (function) _cb = new StaticCallback(function); }

	template<typename T>
	FunctionPointer(T *object, RETURNTYPE (T::*function)(Args...)) : _cb(nullptr)		{ if (object && function) _cb = new MemberCallback<T>(object, function); }

	template<typename T>
	FunctionPointer(T *object, RETURNTYPE(T::*function)(Args...) const) : _cb(nullptr)	{ if (object && function) _cb = new MemberCallback<T>(object, function); }

	FunctionPointer(const FunctionPointer &other) : _cb(nullptr) { operator=(other); }

	FunctionPointer(FunctionPointer &&other) { _cb = other._cb; other._cb = nullptr; }

	~FunctionPointer() { delete[] _cb; }

	inline FunctionPointer& operator=(const FunctionPointer &other)
	{
		if (other._cb)
			_cb = Utilities::CopyOf(*other._cb, other._cb->SizeOf());
		else
			_cb = nullptr;

		return *this;
	}

	inline RETURNTYPE operator()(Args ...args)			{ return _cb->Call(args...); }
	inline RETURNTYPE operator()(Args ...args) const	{ return _cb->Call(args...); }
	inline bool IsCallable() const						{ return _cb != nullptr; }
};

typedef FunctionPointer<void> Callback;

typedef void (Command)(const Buffer<String>& args);
typedef FunctionPointer<void, const Buffer<String>&> CommandPtr;

template<typename T>
using Getter = FunctionPointer<T>;

template<typename T>
using Setter = FunctionPointer<void, const T&>;
