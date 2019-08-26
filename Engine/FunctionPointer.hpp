#pragma once
#include "Debug.hpp"
#include "Utilities.hpp"

class _PtrData
{
public:
	void* obj;
	void* ptr;

	_PtrData(void* obj, void* ptr) : obj(obj), ptr(ptr) {}

	inline bool operator==(const _PtrData& other) const { return ptr == other.ptr && obj == other.obj; }
};

/*
	FunctionPointer

	A pointer to any function, as either a static function or a member function of a specific class instance
*/

template <typename RETURNTYPE, typename ...Args>
class FunctionPointer
{
	class FunctionPointerBase
	{
	protected:
		virtual _PtrData _GetPtrData() const = 0;

	public:
		virtual ~FunctionPointerBase() {}

		virtual RETURNTYPE Call(Args...) = 0;
		virtual RETURNTYPE Call(Args...) const = 0;
		virtual bool IsCallable() const = 0;

		inline void TryCall(Args... args)		{ if (IsCallable()) Call(args...); }
		inline void TryCall(Args... args) const	{ if (IsCallable()) Call(args...); }

		inline bool operator==(const FunctionPointerBase& other) const { return _GetPtrData() == other._GetPtrData(); }
	};

	class FunctionPointerStatic : public FunctionPointerBase
	{
		RETURNTYPE(*_function)(Args...);

	protected:
		virtual _PtrData _GetPtrData() const override { return _PtrData(nullptr, _function); }

	public:
		FunctionPointerStatic(RETURNTYPE(*function)(Args...)) : _function(function) {}
		virtual ~FunctionPointerStatic() {}

		virtual RETURNTYPE Call(Args ...args) override			{ return _function(args...); }
		virtual RETURNTYPE Call(Args ...args) const override	{ return _function(args...); }
		virtual bool IsCallable() const							{ return _function != nullptr; }
	};

	template<typename T>
	class FunctionPointerMember : public FunctionPointerBase
	{
		T* _object;

		union
		{
			RETURNTYPE(T::* _function)(Args...);
			RETURNTYPE(T::* _constFunction)(Args...) const;

			void* _voidPtr;
		};

		bool _isConstFunction;

	protected:
		virtual _PtrData _GetPtrData() const override { return _PtrData(_object, _voidPtr); }
		
	public:
		FunctionPointerMember() : _object(nullptr), _function(nullptr), _isConstFunction(false) {}

		FunctionPointerMember(T* object, RETURNTYPE(T::* function)(Args...)) : 
			_object(object), 
			_function(function), 
			_isConstFunction(false) 
		{}

		FunctionPointerMember(T* object, RETURNTYPE(T::* function)(Args...) const) : 
			_object(object), 
			_constFunction(function), 
			_isConstFunction(true)
		{}

		virtual ~FunctionPointerMember() {}

		virtual RETURNTYPE Call(Args... args) override			{ return _isConstFunction ? (_object->*_constFunction)(args...) : (_object->*_function)(args...); }
		virtual RETURNTYPE Call(Args... args) const override	{ return _isConstFunction ? (_object->*_constFunction)(args...) : (_object->*_function)(args...); }
		virtual bool IsCallable() const							{ return _function != nullptr; }
	};

	/////
	union Union
	{
		FunctionPointerStatic					fptrStatic;
		FunctionPointerMember<FunctionPointer>	fptrMember;

		Union(const FunctionPointerStatic& _static) : fptrStatic(_static) {}

		template<typename T>
		Union(const FunctionPointerMember<T>& _member) : fptrMember() 
		{
			//This is disgostan
			Utilities::CopyBytes(&_member, this, sizeof(_member));
		}

		~Union() {}
	} _u;

	inline FunctionPointerBase& _Fptr()				{ return reinterpret_cast<FunctionPointerBase&>(_u); }
	inline const FunctionPointerBase& _Fptr() const	{ return reinterpret_cast<const FunctionPointerBase&>(_u); }

public:
	FunctionPointer(RETURNTYPE(*function)(Args...) = nullptr) : _u(function) {}

	template<typename T>
	FunctionPointer(T* object, RETURNTYPE(T::* function)(Args...)) : _u(FunctionPointerMember<T>(object, function)) {}

	template<typename T>
	FunctionPointer(T* object, RETURNTYPE(T::* function)(Args...) const) : _u(FunctionPointerMember<T>(object, function)) {}

	FunctionPointer(const FunctionPointer& other) : _u(nullptr) { operator=(other); }

	FunctionPointer(FunctionPointer&& other) { operator=(other); }

	~FunctionPointer() { }

	inline FunctionPointer& operator=(const FunctionPointer& other)
	{
		Utilities::CopyBytes(&other._u, &_u, sizeof(_u));
		return *this;
	}

	inline FunctionPointer& operator=(FunctionPointer&& other)
	{
		Utilities::CopyBytes(&other._u, &_u, sizeof(_u));
		other._Fptr() = FunctionPointerStatic(nullptr);
		return *this;
	}

	inline RETURNTYPE operator()(Args ...args)			{ return _Fptr().Call(args...); }
	inline RETURNTYPE operator()(Args ...args) const	{ return _Fptr().Call(args...); }
	inline bool IsCallable() const						{ return _Fptr().IsCallable(); }
	inline void TryCall(Args ...args)					{ _Fptr().TryCall(args...); }
	inline void TryCall(Args ...args) const				{ _Fptr().TryCall(args...); }

	inline bool operator==(const FunctionPointer& other) const { return (FunctionPointerBase&)_u == (FunctionPointerBase&)other._u; }
};

typedef FunctionPointer<void> Callback;

typedef void (Command)(const Buffer<String>& args);
typedef FunctionPointer<void, const Buffer<String>&> CommandPtr;

template<typename T>
using Getter = FunctionPointer<T>;

template<typename T>
using Setter = FunctionPointer<void, const T&>;
