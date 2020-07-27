#pragma once
#include "Types.hpp"

template <typename T, int SIZE>
class Pool
{
	byte _data[SIZE * sizeof(T)];
	size_t _usedSlotCount;

	T& GetElement(size_t index) { return (T&)_data[index * sizeof(T)]; }

public:
	Pool() : _data(), _usedSlotCount(0) {}

	~Pool()
	{
		for (size_t i = 0; i < _usedSlotCount; ++i)
			GetElement(i).~T();
	}

	void Clear()
	{
		for (size_t i = 0; i < _usedSlotCount; ++i)
			GetElement(i).~T();

		_usedSlotCount = 0;
	}

	bool IsFull() const { return _usedSlotCount >= SIZE; }
	size_t GetUsedSlotCount() const { return _usedSlotCount; }

	template <typename... Args>
	T* New(Args... args)
	{
		return _usedSlotCount < SIZE ? new(_data + (_usedSlotCount++ * sizeof(T))) T(args...) : nullptr;
	}

	T* NewArray(size_t size)
	{
		if (size == 0 || _usedSlotCount + size > SIZE) return nullptr;

		T* mem = new(_data + (_usedSlotCount * sizeof(T))) T[size];
		_usedSlotCount += size;
		return mem;
	}

	void DeleteHandler(T*) const {}
};

#include "List.hpp"

template <typename T, int PAGE_SIZE>
class MultiPool
{
	typedef Pool<T, PAGE_SIZE> _PoolType;

	List<_PoolType> _pools;
	typename List<_PoolType>::Iterator _firstUsablePage;

public:
	MultiPool() {}
	~MultiPool() {}

	void Clear(bool freeMemory = false)
	{
		if (freeMemory)
			_pools.Clear();
		else
		{
			for (_PoolType& pool : _pools)
				pool.Clear();
		}
	}

	template <typename... Args>
	T* New(Args... args)
	{
		for (auto it = _firstUsablePage; it.IsValid(); ++it)
			if (!it->IsFull())
			{
				T* result = it->New(args...);
				if (it->IsFull())
					_firstUsablePage = it.Next();

				return result;
			}

		_firstUsablePage = _pools.Emplace();
		return _firstUsablePage->New(args...);
	}

	T* NewArray(size_t size)
	{
		if (size == 0) return nullptr;

		for (auto it = _firstUsablePage; it.IsValid(); ++it)
			if (PAGE_SIZE - it->GetUsedSlotCount() >= size)
			{
				T* result = it->NewArray(size);
				if (it->IsFull())
					_firstUsablePage = it.Next();

				return result;
			}

		if (_firstUsablePage.IsValid())
			return _pools.Emplace()->NewArray(size);

		_firstUsablePage = _pools.Emplace();
		return _firstUsablePage->NewArray(size);
	}

	void DeleteHandler(T*) const {}
};
