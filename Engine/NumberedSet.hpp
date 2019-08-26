#pragma once
#include "Hashmap.hpp"

typedef uint16 NSInt;

template<typename T>
class NumberedSet 
{
	Hashmap<NSInt, T> _map;
	NSInt nextId;

public:
	NumberedSet(uint32 initialID = 1) : nextId(initialID) {}
	~NumberedSet() {}

	inline T* Get(NSInt id) { return _map.Get(id); }
	inline const T* Get(NSInt id) const { return _map.Get(id); }

	inline const NSInt* IDOf(const T &value) const { return _map.FindFirstKey(value); }

	//Adds item if not present, returns ID
	inline NSInt Add(const T &value) 
	{
		const NSInt* existingID = _map.FindFirstKey(value);
		if (existingID)
			return *existingID;

		_map[nextId] = value;
		return nextId++;
	}

	inline Buffer<Pair<const NSInt, const String>*> ToKVBuffer() const { return _map.ToKVBuffer(); }

	inline T& operator[](NSInt id) { return _map[id]; }
};
