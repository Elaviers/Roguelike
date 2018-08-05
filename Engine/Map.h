#pragma once
#include <utility>		//For move, of course

template <typename KEYTYPE, typename VALUETYPE>
class Map
{
private:
	struct Pair
	{
		Pair(const KEYTYPE &key, const VALUETYPE &value) : key(key), value(value), left(nullptr), right(nullptr) {}

		KEYTYPE key;
		VALUETYPE value;

		Pair *left;
		Pair *right;
	};

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

	void _CallFunctionOnChildren(Pair *pair, void(*function)(VALUETYPE &))
	{
		function(pair->value);

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
	~Map() { if (_data) _DeletePair(_data); }

	void ForEach(void (*function)(VALUETYPE &))
	{
		if (_data)
			_CallFunctionOnChildren(_data, function);
	}

	VALUETYPE* const Find(const KEYTYPE &key)
	{
		if (_data)
		{
			auto pair = _FindPair(_data, key);
			if (pair)
				return &pair->value;
		}

		return nullptr;
	}

	const VALUETYPE* const Find(const KEYTYPE &key) const
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
};
