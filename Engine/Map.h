#pragma once
#include "Buffer.h"
#include <utility>		//For move, of course

#define INVALID_INDEX 0xFFFFFFFF

template <typename KEYTYPE, typename VALUETYPE>
class Map
{
private:
	struct Pair
	{
		Pair() {}
		Pair(const KEYTYPE &key, const VALUETYPE &value) : key(key), value(value) {}

		KEYTYPE key;
		VALUETYPE value;
	};

	Buffer<Pair> _data;

	uint32 FindKeyIndex(const KEYTYPE &key) const
	{
		for (uint32 i = 0; i < _data.GetSize(); ++i)
		{
			if (_data[i].key > key)
				return INVALID_INDEX;

			if (_data[i].key == key)
				return i;
		}

		return INVALID_INDEX;
	}

	Pair& AddPair(const KEYTYPE &key, const VALUETYPE &value)
	{
		uint32 i = 0;
		if (_data.GetSize())
			while (i < _data.GetSize() && key > _data[i].key)
				i++;

		_data.Insert(Pair(key, value), i);
		return _data[i];
	}

public:
	Map() {}
	~Map() {}

	void ForEach(void (*function)(VALUETYPE &))
	{
		for (uint32 i = 0; i < _data.GetSize(); ++i)
			function(_data[i].value);
	}

	VALUETYPE* const Find(const KEYTYPE &key)
	{
		auto index = FindKeyIndex(key);
		if (index != INVALID_INDEX)
			return &_data[index].value;

		return nullptr;
	}

	const VALUETYPE* const Find(const KEYTYPE &key) const
	{
		auto index = FindKeyIndex(key);
		if (index != INVALID_INDEX)
			return &_data[index].value;

		return nullptr;
	}

	void Add(const KEYTYPE &key, const VALUETYPE &value)
	{
		if (FindKeyIndex(key) == INVALID_INDEX)
			AddPair(key, value);
	}

	void Set(const KEYTYPE &key, const VALUETYPE &value)
	{
		auto index = FindKeyIndex(key);

		if (index == INVALID_INDEX)
			AddPair(key, value);
		else
			_data[i].value = value;
	}

	VALUETYPE* New(const KEYTYPE &key)
	{
		if (FindKeyIndex(key) == INVALID_INDEX)
		{
			return &AddPair(key, VALUETYPE()).value;
		}

		return nullptr;
	}
};
