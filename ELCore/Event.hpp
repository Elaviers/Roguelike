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
		for (const FunctionPointer<void, Args...>& fp : _list)
			if (fp == function)
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
		for (const FunctionPointer<void, Args...>& fp : _list)
			fp.TryCall(args...);
	}
};
