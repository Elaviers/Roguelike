#pragma once
#include "Utilities.h"

class Callback
{
	class BaseCallback
	{
	public:
		virtual ~BaseCallback() {}

		virtual void Call() = 0;
		virtual size_t SizeOf() const = 0;
	};

	class StaticCallback : public BaseCallback
	{
		void(*_function)();

	public:
		StaticCallback(void(*function)()) {}
		virtual ~StaticCallback() {}

		virtual void Call() override { if (_function) _function(); }
		virtual size_t SizeOf() const override { return sizeof(*this); }
	};

	template<typename T>
	class MemberCallback : public BaseCallback
	{
		T *_object;
		void(T::*_function)();

	public:
		MemberCallback(T *object, void (T::*function)()) : _object(object), _function(function) {}
		virtual ~MemberCallback() {}

		virtual void Call() override { (_object->*_function)(); }
		virtual size_t SizeOf() const override { return sizeof(*this); }
	};

	/////

	BaseCallback *_cb;

public:
	Callback(void(*function)() = nullptr) : _cb(new StaticCallback(function)) {}

	template<typename T>
	Callback(T *object, void (T::*function)()) : _cb(new MemberCallback<T>(object, function)) {}

	Callback(const Callback &other) { operator=(other); }

	Callback(Callback &&other) { _cb = other._cb; other._cb = nullptr; }

	~Callback() { delete[] _cb; }

	inline Callback& operator=(const Callback &other)
	{
		uint32 size = (uint32)other._cb->SizeOf();
		_cb = (BaseCallback*)new byte[size];
		Utilities::CopyBytes(other._cb, _cb, size);

		return *this;
	}

	inline void operator()() { _cb->Call(); }
};
