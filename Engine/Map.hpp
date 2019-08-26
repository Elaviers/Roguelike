#pragma once
#include "Buffer.hpp"
#include "Pair.hpp"

template <typename K, typename V>
class Map
{
	struct Node
	{
		K key;
		V value;

		Node* left;
		Node* right;

		Node(const K& key, const V &value) : key(key), value(value), left(nullptr), right(nullptr) {}

		~Node()
		{
			if (left) delete left;
			if (right) delete right;
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

		Node& Get(const K& k)
		{
			if (k < key)
			{
				if (left) return left->Get(k);
				return *(left = new Node(k, V()));
			}
			else if (k > key)
			{
				if (right) return right->Get(k);
				return *(right = new Node(k, V()));
			}

			return *this;
		}

		Node& Set(const K& k, const V& v)
		{
			if (k < key)
			{
				if (left) return left->Set(k, v);
				return *(left = new Node(k, v));
			}
			else if (k > key)
			{
				if (right) return right->Set(k, v);
				return *(right = new Node(k, v));
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

		Node* Copy()
		{
			Node* node = new Node(key, value);

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

		void ForEach(void (*function)(const K&, V&))
		{
			function(key, value);
			if (left)	left->ForEach(function);
			if (right)	right->ForEach(function);
		}
	};

	Node *_data;

public:
	Map() : _data(nullptr) {}
	~Map() { delete _data; }

	Map(const Map &other) : _data(nullptr) { if (other._data) _data = other._data->Copy(); }
	Map(Map&& other) : _data(other._data) { other._data = nullptr; }

	inline void Clear()				{ delete _data; _data = nullptr; }
	inline uint32 GetSize() const	{ return _data ? _data->Count() : 0; }
	inline bool IsEmpty() const		{ return _data == nullptr; }

	const K* const FindFirstKey(const V &value) const
	{
		if (_data)
		{
			auto node = _data->GetWithValue(value);
			if (node) return &node->key;
		}

		return nullptr;
	}

	const V* Get(const K& key) const
	{
		if (_data)
		{
			auto node = _data->Find(key);
			if (node) return &node->value;
		}

		return nullptr;
	}

	V& operator[](const K& key)
	{
		if (_data) return _data->Get(key).value;

		return Set(key, V());
	}

	V& Set(const K &key, const V &value)
	{
		if (_data) return _data->Set(key, value).value;
		
		_data = new Node(key, value);
		return _data->value;
	}

	inline V* Get(const K& key) { return const_cast<V*>(((const Map*)this)->Get(key)); }

	inline Map& operator=(const Map &other)
	{
		Clear();
		if (other._data) _data = other._data->Copy();
		return *this;
	}

	inline Buffer<const K*> ToKBuffer() const
	{
		Buffer<const K*> buffer;
		if (_data) _data->AddToK(buffer);
		return buffer;
	}

	inline Buffer<Pair<const K*, const V*>> ToKVBuffer() const
	{
		Buffer<Pair<const K*, const V*>> buffer;
		if (_data) _data->AddTo(buffer);
		return buffer;
	}

	inline void ForEach(void (*function)(const K&, V&))
	{
		if (_data) _data->ForEach(function);
	}
};
