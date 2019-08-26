#pragma once
#include "List.hpp"
#include "Pair.hpp"
#include "SharedPointer.hpp"

template<typename T>
class Tracker
{
	List<SharedPointerData<T>> _list;

	SharedPointerData<T>* _Find(T* addr)
	{
		for (auto it = _list.First(); it; ++it)
			if (it->ptr == addr)
				return &*it;

		return nullptr;
	}

	void _Remove(SharedPointerData<T> &pd)
	{
		for (auto it = _list.First(); it; ++it)
			if (it->ptr == pd.ptr)
			{
				_list.Remove(it);
				return;
			}
	}

public:
	Tracker() {}
	~Tracker() {}

	SharedPointerC<T> Track(T* addr)
	{
		SharedPointerData<T>* existing = _Find(addr);
		if (existing == nullptr)
			existing = &*_list.Add(SharedPointerData<T>(addr, 1, FunctionPointer<void, SharedPointerData<T>&>(this, &Tracker::_Remove)));
		else
			++existing->referenceCount;

		return SharedPointerC<T>(*existing);
	}
	
	void Null(T* addr)
	{
		SharedPointerData<T>* existing = _Find(addr);

		if (existing)
			existing->ptr = nullptr;
	}
};
