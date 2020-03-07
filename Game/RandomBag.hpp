#pragma once
#include <Engine/Buffer.hpp>
#include <Engine/Random.hpp>

template <typename T, typename W = float>
class RandomBag
{
public:
	struct Entry
	{
		T item;
		W weight;

		Entry() {}

		Entry(const T& item, const W& weight) : item(item), weight(weight) {}
		
		template <typename... Args>
		Entry(const W& weight, Args... args) : weight(weight), item(args...) {}
	};

private:
	Buffer<Entry> _entries;

	W _remainingWeight;

	Entry& _Next(Random& random)
	{
		float weight = random.NextFloat() * _remainingWeight;
		size_t index = 0;

		while (weight > _entries[index].weight)
		{
			weight -= _entries[index].weight;
			index++;
		}

		return _entries[index];
	}

public:
	RandomBag() : _remainingWeight((W)0) {}
	~RandomBag() {}

	void Clear()
	{
		_entries.Clear();
		_remainingWeight = 0.f;
	}

	const Buffer<Entry>& GetEntries() const { return _entries; }

	const W& GetRemainingWeight() const { return _remainingWeight; }
	void SetRemainingWeight(const W& remainingWeight) { _remainingWeight = remainingWeight; }

	void Add(const T& item, float weight)
	{
		_remainingWeight += weight;
		_entries.Add({item, weight});
	}

	template <typename... Args>
	void Emplace(float weight, Args... args)
	{
		_remainingWeight += weight;
		_entries.Emplace(weight, args...);
	}

	T& PeekNext(Random& random) {return _Next(random).item; }
	const T& PeekNext(Random& random) const { return _Next(random).item; }
	
	void TakeFrom(const T& item, const W& weightToTake = (W)1)
	{
		for (size_t i = 0; i < _entries.GetSize(); ++i)
		{
			if (_entries[i].item == item)
			{
				if (weightToTake == (W)0)
				{
					_remainingWeight -= _entries[i].weight;
					_entries[i].weight = 0.f;
				}
				else
				{
					_remainingWeight -= weightToTake;
					_entries[i].weight -= weightToTake;
				}
			}

			return;
		}
	}

	/*
		if weightToTake is 0, effectively removes item from bag
	*/
	T& TakeNext(Random& random, W weightToTake = (W)1)
	{
		Entry& entry = _Next(random);

		if (weightToTake == (W)0)
		{
			_remainingWeight -= entry.weight;
			entry.weight = 0.f;
		}
		else
		{
			_remainingWeight -= weightToTake;
			entry.weight -= weightToTake;
		}

		return entry.item;
	}
};

