#pragma once
#include "Utilities.h"

template <typename ...Args>
class FPTR
{
	class BaseCallback
	{
	public:
		virtual ~BaseCallback() {}

		virtual void Call(Args...) = 0;
		virtual size_t SizeOf() const = 0;
	};

	class StaticCallback : public BaseCallback
	{
		void(*_function)(Args...);

	public:
		StaticCallback(void(*function)(Args...)) {}
		virtual ~StaticCallback() {}

		virtual void Call(Args ...args) override { if (_function) _function(args...); }
		virtual size_t SizeOf() const override { return sizeof(*this); }
	};

	template<typename T>
	class MemberCallback : public BaseCallback
	{
		T *_object;
		void(T::*_function)(Args...);

	public:
		MemberCallback(T *object, void (T::*function)(Args...)) : _object(object), _function(function) {}
		virtual ~MemberCallback() {}

		virtual void Call(Args... args) override { (_object->*_function)(args...); }
		virtual size_t SizeOf() const override { return sizeof(*this); }
	};

	/////

	BaseCallback *_cb;

public:
	FPTR(void(*function)(Args...) = nullptr) : _cb(new StaticCallback(function)) {}

	template<typename T>
	FPTR(T *object, void (T::*function)(Args...)) : _cb(new MemberCallback<T>(object, function)) {}

	FPTR(const FPTR &other) { operator=(other); }

	FPTR(FPTR &&other) { _cb = other._cb; other._cb = nullptr; }

	~FPTR() { delete[] _cb; }

	inline FPTR& operator=(const FPTR &other)
	{
		uint32 size = (uint32)other._cb->SizeOf();
		_cb = (BaseCallback*)new byte[size];
		Utilities::CopyBytes(other._cb, _cb, size);

		return *this;
	}

	inline void operator()(Args ...args) { _cb->Call(args...); }
};

typedef FPTR<> Callback;
typedef FPTR<const Buffer<String>&> Command;
