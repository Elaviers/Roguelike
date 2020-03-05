#pragma once
#include "Buffer.hpp"
#include "FunctionPointer.hpp"
#include "Pair.hpp"

template <typename K, typename V>
class Map
{
	NewHandler _handlerNew;
	DeleteHandler _handlerDelete;

	class Node;

	Node* _NewNode(const K& key, const V& value) const
	{
		if (_handlerNew.IsCallable())
			return new (_handlerNew(sizeof(Node))) Node(key, value);

		return new Node(key, value);
	}

	void _DeleteNode(Node* node) const
	{
		if (_handlerDelete.IsCallable())
		{
			node->~Node();
			_handlerDelete((byte*)node);
			return;
		}

		delete node;
	}

	struct Node
	{
		K key;
		V value;

		Node* left;
		Node* right;

		Node(const K& key, const V &value) : key(key), value(value), left(nullptr), right(nullptr) {}
		~Node() {}

		void DeleteChildren(const Map& map) const
		{
			if (left) map._DeleteNode(left);
			if (right) map._DeleteNode(right);
		}

		uint32 Count() const
		{
			uint32 amount = 1;

			if (left) amount += left->Count();
			if (right) amount += right->Count();

			return amount;
		}

		Node* Find(const K& k)
		{
			if (k < key)
			{
				if (left) return left->Find(k);
				return nullptr;
			}
			else if (k > key)
			{
				if (right) return right->Find(k);
				return nullptr;
			}

			return this;
		}

		Node& Get(const K& k, const Map& map)
		{
			if (k < key)
			{
				if (left) return left->Get(k, map);
				return *(left = map._NewNode(k, V()));
			}
			else if (k > key)
			{
				if (right) return right->Get(k, map);
				return *(right = map._NewNode(k, V()));
			}

			return *this;
		}

		Node& Set(const K& k, const V& v, const Map& map)
		{
			if (k < key)
			{
				if (left) return left->Set(k, v, map);
				return *(left = map._NewNode(k, v));
			}
			else if (k > key)
			{
				if (right) return right->Set(k, v, map);
				return *(right = map._NewNode(k, v));
			}

			value = v;
			return *this;
		}

		Node* GetWithValue(const V& v)
		{
			if (v == value)
				return this;

			Node* pair;
			if (left &&		(pair = left->GetWithValue(v)))		return pair;
			if (right &&	(pair = right->GetWithValue(v)))	return pair;
			return nullptr;
		}

		Node* Copy(const Map& map)
		{
			Node* node = map._NewNode(key, value);

			if (left)	node->left = left->Copy();
			if (right)	node->right = right->Copy();

			return node;
		}

		void AddToK(Buffer<const K*>& buffer) const
		{
			buffer.Add(&key);

			if (left)	left->AddToK(buffer);
			if (right)	right->AddToK(buffer);
		}

		void AddTo(Buffer<Pair<const K*, const V*>>& buffer) const
		{
			buffer.Add(Pair<const K*, const V*>(&key, &value));

			if (left)	left->AddTo(buffer);
			if (right)	right->AddTo(buffer);
		}

		template <typename Function>
		void ForEach(Function function)
		{
			function((const K&)key, (V&)value);
			if (left)	left->ForEach(function);
			if (right)	right->ForEach(function);
		}
	};

	Node *_data;

public:
	Map() : _data(nullptr) {}
	~Map() 
	{ 
		if (_data)
		{
			_data->DeleteChildren(*this);
			_DeleteNode(_data);
		}
	}

	Map(const Map& other) : _data(nullptr)	{ if (other._data) _data = other._data->Copy(); }
	Map(Map&& other) : _data(other._data)	
	{
		other._data = nullptr;

		if (other._handlerNew != _handlerNew || other._handlerDelete != _handlerDelete)
			operator=((const Map&)*this);
	}

	void Clear()				
	{ 
		if (_data)
		{
			_data->DeleteChildren(*this);
			_DeleteNode(_data);
			_data = nullptr;
		}
	}

	uint32 GetSize() const		{ return _data ? _data->Count() : 0; }
	bool IsEmpty() const		{ return _data == nullptr; }

	const K* const FindFirstKey(const V &value) const
	{
		if (_data)
		{
			auto node = _data->GetWithValue(value);
			if (node) return &node->key;
		}

		return nullptr;
	}

	V* Get(const K& key)
	{
		if (_data)
		{
			auto node = _data->Find(key);
			if (node) return &node->value;
		}

		return nullptr;
	}

	const V* Get(const K& key) const { return (const V*)const_cast<Map*>(this)->Get(key); }

	V& operator[](const K& key)
	{
		if (_data) return _data->Get(key, *this).value;

		return Set(key, V());
	}

	V& Set(const K &key, const V &value)
	{
		if (_data) return _data->Set(key, value, *this).value;
		
		_data = _NewNode(key, value);
		return _data->value;
	}

	Map& operator=(const Map &other)
	{
		Clear();
		if (other._data) _data = other._data->Copy(*this);
		return *this;
	}

	Map& operator=(Map&& other)
	{
		_data = other._data;
		other._data = nullptr;

		if (other._handlerNew != _handlerNew || other._handlerDelete != _handlerDelete)
			return operator=((const Map&)*this);

		return *this;
	}

	Buffer<const K*> ToKBuffer() const
	{
		Buffer<const K*> buffer;
		if (_data) _data->AddToK(buffer);
		return buffer;
	}

	Buffer<Pair<const K*, const V*>> ToKVBuffer() const
	{
		Buffer<Pair<const K*, const V*>> buffer;
		if (_data) _data->AddTo(buffer);
		return buffer;
	}

	//args must be (const K&, V&)
	template <typename Function>
	void ForEach(Function function)
	{
		if (_data) _data->ForEach(function);
	}
};
