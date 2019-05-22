#pragma once
#include "Buffer.hpp"
#include "Pair.hpp"

template <typename KEYTYPE, typename VALUETYPE>
class Map
{
public:
	struct MapNode
	{
		MapNode(const KEYTYPE &key, const VALUETYPE &value) : key(key), value(value), left(nullptr), right(nullptr) {}

		KEYTYPE key;
		VALUETYPE value;

		MapNode *left;
		MapNode *right;
	};

private:
	MapNode *_data;

	MapNode* _FindNode(MapNode *current, const KEYTYPE &key) const //Hmm
	{
		if (current->key == key)
			return current;

		if (key < current->key)
		{
			if (current->left)
				return _FindNode(current->left, key);
		}
		else if (current->right  && key > current->key)
			return _FindNode(current->right, key);

		return nullptr;
	}

	MapNode* _FindFirstNodeWithValue(MapNode *current, const VALUETYPE &value) const
	{
		if (current->value == value)
			return current;

		MapNode *pair;

		if (current->left && (pair = _FindFirstNodeWithValue(current->left, value)) != nullptr)
			return pair;

		if (current->right && (pair = _FindFirstNodeWithValue(current->right, value)) != nullptr)
			return pair;

		return nullptr;
	}

	MapNode& _Set(MapNode *current, const KEYTYPE &key, const VALUETYPE &value)
	{
		if (key < current->key)
		{
			if (current->left == nullptr)
				return *(current->left = new MapNode(key, value));
			
			return _Set(current->left, key, value);
		}
		if (key > current->key)
		{
			if (current->right == nullptr)
				return *(current->right = new MapNode(key, value));

			return _Set(current->right, key, value);
		}

		current->value = value;
		return *current;
	}

	void _CallFunctionOnChildren(MapNode *node, void (*function)(const KEYTYPE&, VALUETYPE&))
	{
		function(node->key, node->value);

		if (node->left)
			_CallFunctionOnChildren(node->left, function);
		if (node->right)
			_CallFunctionOnChildren(node->right, function);
	}

	void _DeleteNode(MapNode *node)
	{
		if (node->left)
			_DeleteNode(node->left);
		if (node->right)
			_DeleteNode(node->right);

		delete node;
	}

	MapNode* _CopyNode(MapNode *src)
	{
		MapNode *newPair = new MapNode(src->key, src->value);

		if (src->left)
			newPair->left = _CopyNode(src->left);
		if (src->right)
			newPair->right = _CopyNode(src->right);

		return newPair;
	}

	void _AddNodeToBuffer(const MapNode *node, Buffer<Pair<const KEYTYPE*, const VALUETYPE*>> &buffer) const
	{
		buffer.Add(Pair<const KEYTYPE*, const VALUETYPE*>(&node->key, &node->value));

		if (node->left)
			_AddNodeToBuffer(node->left, buffer);
		if (node->right)
			_AddNodeToBuffer(node->right, buffer);
	}

public:
	Map() : _data(nullptr) {}
	~Map() { Clear(); }

	Map(const Map &other) : _data(nullptr)
	{
		if (other._data) _data = _CopyNode(other._data);
	}

	Map(Map&& other) : _data(other._data) { other._data = nullptr; }
	
	inline MapNode* GetFirstNode() { return _data; }

	void ForEach(void (*function)(const KEYTYPE&, VALUETYPE&))
	{
		if (_data)
			_CallFunctionOnChildren(_data, function);
	}

	const KEYTYPE* const FindFirstKey(const VALUETYPE &value) const
	{
		if (_data)
		{
			auto node = _FindFirstNodeWithValue(_data, value);
			if (node)
				return &node->key;
		}

		return nullptr;
	}

	const VALUETYPE* Find(const KEYTYPE &key) const
	{
		if (_data)
		{
			auto node = _FindNode(_data, key);
			if (node)
				return &node->value;
		}

		return nullptr;
	}

	inline VALUETYPE* Find(const KEYTYPE &key) { return const_cast<VALUETYPE*>(((const Map*)this)->Find(key)); }

	VALUETYPE& Set(const KEYTYPE &key, const VALUETYPE &value)
	{
		if (_data)
			return _Set(_data, key, value).value;
		
		_data = new MapNode(key, value);
		return _data->value;
	}

	VALUETYPE& operator[](const KEYTYPE &key)
	{
		if (_data)
		{
			MapNode *node = _FindNode(_data, key);
			if (node)
				return node->value;
		}
		
		return Set(key, VALUETYPE());
	}

	inline void Clear()
	{
		if (_data)
		{
			_DeleteNode(_data);
			_data = nullptr;
		}
	}

	inline Map& operator=(const Map &other)
	{
		Clear();

		if (other._data) _data = _CopyNode(other._data);
		return *this;
	}

	inline Buffer<Pair<const KEYTYPE*, const VALUETYPE*>> ToBuffer() const
	{
		Buffer<Pair<const KEYTYPE*, const VALUETYPE*>> buffer;

		if (_data)
			_AddNodeToBuffer(_data, buffer);

		return buffer;
	}
};
