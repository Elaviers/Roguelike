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

	inline Event& operator+=(const FunctionPointer<void, Args...>& function) 
	{
		for (auto it = _list.First(); it; ++it)
			if (*it == function)
				return *this;

		_list.Add(function);
		return *this;
	}


	inline Event& operator-=(const FunctionPointer<void, Args...>& function) 
	{ 
		_list.Remove(function);
		return *this;
	}

	inline void operator()(Args... args) const 
	{
		for (auto it = _list.First(); it; ++it)
			it->TryCall();
	}
};
