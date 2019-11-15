#pragma once
#include "FunctionPointer.hpp"
#include "Types.hpp"

template <typename T>
class SharedPointerData
{
public:
	T* ptr;
	uint32 referenceCount;
	FunctionPointer<void, SharedPointerData&> onZeroReferences;

	SharedPointerData(T* addr, uint32 referenceCount, const FunctionPointer<void, SharedPointerData&>& onZeroReferences = FunctionPointer<void, SharedPointerData&>()) :
		ptr(addr),
		referenceCount(referenceCount),
		onZeroReferences(onZeroReferences)
	{}

	inline void Decrement()
	{
		--referenceCount;

		if (referenceCount <= 0)
			onZeroReferences.TryCall(*this);
	}

	inline void Increment()
	{
		++referenceCount;
	}
};

extern SharedPointerData<void> _nullPtrData;

/*
	SharedPointerC
	Base class

	By default, this pointer does absolutely nothing when nothing refers to it.
	Use SharedPointer for expected behaviour
*/
template <typename T>
class SharedPointerC
{
private:
	//Should never be nullptr
	SharedPointerData<T>* _data;

	inline SharedPointerData<T>* _NullPtrData() const { return reinterpret_cast<SharedPointerData<T>*>(&_nullPtrData); }

public:
	SharedPointerC() : _data(_NullPtrData()) {}
	SharedPointerC(SharedPointerData<T>& data) : _data(&data) {}

	SharedPointerC(SharedPointerC& other) : _data(other._data)
	{
		_data->Increment();
	}

	SharedPointerC(SharedPointerC&& other) : _data(other._data)
	{
		other._data = _NullPtrData();
	}

	~SharedPointerC() { _data->Decrement(); }

	inline void Clear() { if (_data != _NullPtrData()) operator=(*_NullPtrData()); }

	SharedPointerC& operator=(SharedPointerC& other)
	{
		if (other._data != _data)
		{
			_data->Decrement();
			_data = other._data;
			_data->Increment();
		}

		return *this;
	}

	SharedPointerC& operator=(SharedPointerC&& other) noexcept
	{
		_data->Decrement();
		_data = other._data;
		other._data = _NullPtrData();

		return *this;
	}

	inline bool operator==(const T* other) const				{ return _data->ptr == other;}
	inline bool operator==(const SharedPointerC& other) const	{ return _data == other._data; }

	inline T& operator*()				{ return *_data->ptr; }
	inline const T& operator*() const	{ return *_data->ptr; }
	inline T* operator->()				{ return _data->ptr; }
	inline const T* operator->() const	{ return _data->ptr; }

	inline T* Ptr()						{ return _data->ptr; }
	inline const T* Ptr() const			{ return _data->ptr; }

	inline operator bool() const		{ return _data->ptr; }
};

/*
	SharedPointer

	Reference-tracking pointer
	Note: deletes pointer when all references are out of scope
*/
template <typename T>
class SharedPointer : SharedPointerC<T>
{
	static void _DeletePointer(SharedPointerData<T>& data)
	{
		delete data.ptr;
		delete &data;
	}

	const static FunctionPointer<void, SharedPointerData<T>&> _pDeletePointer;

public:
	SharedPointer(T *addr = nullptr) : SharedPointer(addr, _pDeletePointer, 1) {}
};

class Entity;
typedef SharedPointerC<Entity> EntityPointer;
