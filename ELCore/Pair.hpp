#pragma once

template <typename T1, typename T2 = T1>
struct Pair
{
	T1 first;
	T2 second;

	Pair() : first(), second() {}
	Pair(const T1& first) : first(first), second() {}
	Pair(const T1 &first, const T2 &second) : first(first), second(second) {}
};
