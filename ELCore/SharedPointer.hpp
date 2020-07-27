#pragma once
#include "FunctionPointer.hpp"
#include "Types.hpp"

template<typename T>
class SharedPointer;

template <typename T>
class SharedPointerData
{
	friend SharedPointer<T>;

	T* _ptr;
	uint32 _referenceCount;
	FunctionPointer<void, SharedPointerData&> _onZeroReferences;

	void _Increment() { if (_ptr) ++_referenceCount; }

	void _Decrement()
	{
		if (_ptr == nullptr)
			return;

		--_referenceCount;

		if (_referenceCount == 0)
			_onZeroReferences.TryCall(*this);
	}

public:
	SharedPointerData(T* addr, uint32 referenceCount, const FunctionPointer<void, SharedPointerData&>& onZeroReferences = FunctionPointer<void, SharedPointerData&>()) :
		_ptr(addr),
		_referenceCount(referenceCount),
		_onZeroReferences(onZeroReferences)
	{}

	T* GetPtr()							{ return _ptr; }
	const T* GetPtr() const				{ return _ptr; }
	uint32 GetReferenceCount() const	{ return _referenceCount; }

	void SetPtr(T* ptr)					{ _ptr = ptr; }
	void SetOnZeroReferences(const FunctionPointer<void, SharedPointerData&>& onZeroReferences) { _onZeroReferences = onZeroReferences; }
};

extern SharedPointerData<void> _nullPtrData;

template <typename T>
class SharedPointer
{
protected:
	//Should never be nullptr
	SharedPointerData<T>* _data;
	
	static const FunctionPointer<void, SharedPointerData<T>&> _pDeletePointer;
	static void _DeletePointer(SharedPointerData<T>& data)
	{
		delete data.GetPtr();
		delete &data;
	}
	
	static SharedPointerData<T>* _NullPtrData() { return reinterpret_cast<SharedPointerData<T>*>(&_nullPtrData); }

public:
	SharedPointer() : _data(_NullPtrData()) {}
	SharedPointer(const SharedPointer& other) : _data(other._data)
	{
		_data->_Increment();
	}

	SharedPointer(SharedPointer&& other) : _data(other._data)
	{
		other._data = _NullPtrData();
	}
	
	SharedPointer(SharedPointerData<T>& data) : _data(&data) 
	{
		_data->_Increment();
	}

	/*
		Note: addr will be deleted when all SharedPointer instances are out of scope
	*/
	explicit SharedPointer(T* addr)
	{
		_data = addr ? new SharedPointerData<T>(addr, 1, _pDeletePointer) : _NullPtrData();
	}

	~SharedPointer() { _data->_Decrement(); }

	void Clear() { if (_data != _NullPtrData()) operator=(*_NullPtrData()); }

	SharedPointer& operator=(const SharedPointer& other)
	{
		if (other._data != _data)
		{
			_data->_Decrement();
			_data = other._data;
			_data->_Increment();
		}

		return *this;
	}

	SharedPointer& operator=(SharedPointer&& other) noexcept
	{
		_data->_Decrement();
		_data = other._data;
		other._data = _NullPtrData();

		return *this;
	}

	bool operator==(const T* other) const				{ return _data->GetPtr() == other;}
	bool operator==(const SharedPointer& other) const	{ return _data == other._data; }

	T& operator*() const		{ return *_data->GetPtr(); }
	T* operator->() const		{ return _data->GetPtr(); }
	T* Ptr() const				{ return _data->GetPtr(); }
	
	operator bool() const		{ return _data->GetPtr() != nullptr; }

	//
	operator SharedPointer<const T>&() const { return *static_cast<SharedPointer<const T>*>((void*)this); }

	template<typename T2>
	SharedPointer<T2> Cast() const
	{
		if (dynamic_cast<T2*>(_data->GetPtr()))
			return SharedPointer<T2>(*static_cast<const SharedPointer<T2>*>((void*)this));

		return SharedPointer<T2>();
	}
};

template<typename T>
const FunctionPointer<void, SharedPointerData<T>&> SharedPointer<T>::_pDeletePointer(&SharedPointer<T>::_DeletePointer);

class Entity;
typedef SharedPointer<Entity> EntityPointer;
