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
		for (SharedPointerData<T>& sp : _list)
			if (sp.GetPtr() == addr)
				return &sp;

		return nullptr;
	}

	void _Remove(SharedPointerData<T> &pd)
	{
		if (pd.GetPtr())
			return;

		for (auto it = _list.begin(); it.IsValid(); ++it)
			if (it->GetPtr() == pd.GetPtr())
			{
				_list.Remove(it);
				return;
			}
	}

public:
	Tracker() {}
	~Tracker() {}

	SharedPointer<T> Track(T* addr)
	{
		SharedPointerData<T>* pointerData = _Find(addr);
		if (pointerData == nullptr)
			pointerData = &*_list.Add(SharedPointerData<T>(addr, 0, FunctionPointer<void, SharedPointerData<T>&>(this, &Tracker::_Remove)));
		
		return SharedPointer<T>(*pointerData);
	}
	
	void Null(T* addr)
	{
		SharedPointerData<T>* existing = _Find(addr);

		if (existing)
		{
			existing->SetPtr(nullptr);
		
			if (existing->GetReferenceCount() == 0)
				_Remove(*existing);
		}
	}
};
