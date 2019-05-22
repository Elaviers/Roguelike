#pragma once
#include "Map.hpp"

typedef uint16 NSInt;

template<typename T>
class NumberedSet 
{
	Map<NSInt, T> _map;
	NSInt nextId;

public:
	NumberedSet(uint32 initialID = 1) : nextId(initialID) {}
	~NumberedSet() {}

	inline T* Find(NSInt id) { return _map.Find(id); }
	inline const T* Find(NSInt id) const { return _map.Find(id); }

	inline uint32* IDOf(const T &value) { return _map.FindFirstKey(value); }
	inline const uint32* IDOf(const T &value) const { return _map.FindFirstKey(value); }

	//Adds item if not present, returns ID
	inline NSInt Add(const T &value) 
	{
		const NSInt* existingID = _map.FindFirstKey(value);
		if (existingID)
			return *existingID;

		_map[nextId] = value;
		return nextId++;
	}

	inline Buffer<Pair<const NSInt*, const String*>> ToBuffer() const { return _map.ToBuffer(); }

	inline T& operator[](NSInt id) { return _map[id]; }
};
