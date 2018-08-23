#pragma once
#include <utility>		//For move, of course

template <typename KEYTYPE, typename VALUETYPE>
class Map
{
public:
	struct Pair
	{
		Pair(const KEYTYPE &key, const VALUETYPE &value) : key(key), value(value), left(nullptr), right(nullptr) {}

		KEYTYPE key;
		VALUETYPE value;

		Pair *left;
		Pair *right;
	};

private:
	Pair *_data;

	Pair* _FindPair(Pair *current, const KEYTYPE &key) const //Hmm
	{
		if (current->key == key)
			return current;

		if (key < current->key)
		{
			if (current->left)
				return _FindPair(current->left, key);
		}
		else if (current->right  && key > current->key)
			return _FindPair(current->right, key);

		return nullptr;
	}

	Pair* _FindFirstPairWithValue(Pair *current, const VALUETYPE &value) const
	{
		if (current->value == value)
			return current;

		Pair *pair;

		if (current->left && (pair = _FindFirstPairWithValue(current->left, value)) != nullptr)
			return pair;

		if (current->right && (pair = _FindFirstPairWithValue(current->right, value)) != nullptr)
			return pair;

		return nullptr;
	}

	Pair& _Set(Pair *current, const KEYTYPE &key, const VALUETYPE &value)
	{
		if (key < current->key)
		{
			if (current->left == nullptr)
				return *(current->left = new Pair(key, value));
			
			return _Set(current->left, key, value);
		}
		if (key > current->key)
		{
			if (current->right == nullptr)
				return *(current->right = new Pair(key, value));

			return _Set(current->right, key, value);
		}

		current->value = value;
		return *current;
	}

	void _CallFunctionOnChildren(Pair *pair, void (*function)(const KEYTYPE&, VALUETYPE&))
	{
		function(pair->key, pair->value);

		if (pair->left)
			_CallFunctionOnChildren(pair->left, function);
		if (pair->right)
			_CallFunctionOnChildren(pair->right, function);
	}

	void _DeletePair(Pair *pair)
	{
		if (pair->left)
			_DeletePair(pair->left);
		if (pair->right)
			_DeletePair(pair->right);

		delete pair;
	}

public:
	Map() {}
	~Map() { Clear(); }

	void ForEach(void (*function)(const KEYTYPE&, VALUETYPE&))
	{
		if (_data)
			_CallFunctionOnChildren(_data, function);
	}

	const KEYTYPE* const FindFirstKey(const VALUETYPE &value) const
	{
		if (_data)
		{
			auto pair = _FindFirstPairWithValue(_data, value);
			if (pair)
				return &pair->key;
		}

		return nullptr;
	}

	VALUETYPE* Find(const KEYTYPE &key)
	{
		if (_data)
		{
			auto pair = _FindPair(_data, key);
			if (pair)
				return &pair->value;
		}

		return nullptr;
	}

	const VALUETYPE* Find(const KEYTYPE &key) const
	{
		if (_data)
		{
			auto pair = _FindPair(_data, key);
			if (pair)
				return &pair->value;
		}

		return nullptr;
	}

	VALUETYPE& Set(const KEYTYPE &key, const VALUETYPE &value)
	{
		if (_data)
			return _Set(_data, key, value).value;
		
		_data = new Pair(key, value);
		return _data->value;
	}

	VALUETYPE& operator[](const KEYTYPE &key)
	{
		if (_data)
		{
			Pair *pair = _FindPair(_data, key);
			if (pair)
				return pair->value;
		}
		
		return Set(key, VALUETYPE());
	}

	inline void Clear()
	{
		if (_data)
		{
			_DeletePair(_data);
			_data = nullptr;
		}
	}
};
