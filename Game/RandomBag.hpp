#pragma once
#include <Engine/Buffer.hpp>
#include <Engine/Random.hpp>

template <typename T, typename W = float>
class RandomBag
{
	struct Entry
	{
		T item;
		W weight;
	};

	Buffer<Entry> _entries;

	W _totalWeight;

	Entry& _Next(Random& random)
	{
		float weight = random.NextFloat() * _totalWeight;
		size_t index = 0;

		while (weight > _entries[index].weight)
		{
			weight -= _entries[index].weight;
			index++;
		}

		return _entries[index];
	}

public:
	RandomBag() : _totalWeight((W)0) {}
	~RandomBag() {}

	W GetTotalWeight() const { return _totalWeight; }

	void Add(const T& item, float weight)
	{
		_totalWeight += weight;

		for (size_t i = 0; i < _entries.GetSize(); ++i)
		{
			if (_entries[i].item == item)
			{
				_entries[i].weight += weight;
				return;
			}
		}

		_entries.Add({item, weight});
	}

	T& GetNext(Random& random) {return _Next(random).item; }
	const T& GetNext() const { return (T&)const_cast<RandomBag*>(this)->GetNext(); }

	/*
		if weightToTake is 0, effectively removes item from bag
	*/
	T& TakeNext(Random& random, W weightToTake = (W)0)
	{
		Entry& next = _Next(random);

		if (weightToTake == (W)0)
		{
			_totalWeight -= next.weight;
			next.weight = 0.f;
		}
		else
		{
			_totalWeight -= weightToTake;
			next.weight -= weightToTake;
		}

		return next.item;
	}
};

