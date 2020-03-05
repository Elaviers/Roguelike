#pragma once
#include "FunctionPointer.hpp"
#include "List.hpp"

/*
	Event

	Linked list of function pointers
*/
template<typename ...Args>
class Event
{
	List<FunctionPointer<void, Args...>> _list;

public:
	Event() {}
	~Event() {}

	Event& operator+=(const FunctionPointer<void, Args...>& function) 
	{
		for (auto it = _list.First(); it.IsValid(); ++it)
			if (*it == function)
				return *this;

		_list.Add(function);
		return *this;
	}


	bool Remove(const FunctionPointer<void, Args...>& function)
	{
		return _list.Remove(function);
	}

	Event& operator-=(const FunctionPointer<void, Args...>& function)
	{
		Remove(function);
		return *this;
	}

	void operator()(Args... args) const 
	{
		for (auto it = _list.First(); it.IsValid(); ++it)
			it->TryCall(args...);
	}
};
