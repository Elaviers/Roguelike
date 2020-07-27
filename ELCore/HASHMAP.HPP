#pragma once
#include "Buffer.hpp"
#include "FunctionPointer.hpp"
#include "List.hpp"
#include "Hashing.hpp"
#include "Pair.hpp"
#include "Types.hpp"

//For string hashing
#include "String.hpp"

template <typename T>
class Hasher
{
public:
	template <typename K>
	static T Hash(const K& key) { return Hashing::MurmurHash2(&key, sizeof(key)); }

	template<>
	static T Hash<String>(const String& key) { return Hashing::MurmurHash2(key.GetData(), key.GetLength()); }
};

template <typename K, typename V, typename H_TYPE = uint32>
class Hashmap
{
	NewHandler _handlerNew;
	DeleteHandler _handlerDelete;

	class Node;
	Node* _NewNode(H_TYPE hash) const
	{
		if (_handlerNew.IsCallable())
			return new (_handlerNew(sizeof(Node))) Node(hash, _handlerNew, _handlerDelete);

		return new Node(hash, _handlerNew, _handlerDelete);
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

	static H_TYPE _Hash(const K& key) { return Hasher<H_TYPE>::template Hash<K>(key); }

	typedef Pair<const K, V> KVPair;
	typedef Pair<const K, const V> KVPairConst;

	class Node
	{
	public:
		const H_TYPE hash;
		List<KVPair> keys;
		
		Node *left, *right;

		Node(H_TYPE hash, const NewHandler& newHandler, const DeleteHandler& deleteHandler) : hash(hash), keys(newHandler, deleteHandler), left(nullptr), right(nullptr) {}
		~Node() {}

		void DeleteChildren(const Hashmap& map) const
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

		V* GetValueForKey(const K &key)
		{
			for (KVPair& kv : keys)
				if (kv.first == key)
					return &kv.second;

			return nullptr;
		}

		Node* Find(H_TYPE h)
		{
			if (h < hash)
			{
				if (left) return left->Find(h);
				return nullptr;
			}
			else if (h > hash)
			{
				if (right) return right->Find(h);
				return nullptr;
			}

			return this;
		}

		Node& Get(H_TYPE h, const Hashmap& map)
		{
			if (h < hash)
			{
				if (left) return left->Get(h, map);
				return *(left = map._NewNode(h));
			}
			else if (h > hash)
			{
				if (right) return right->Get(h, map);
				return *(right = map._NewNode(h));
			}

			return *this;
		}

		KVPair* GetWithValue(const V& v)
		{
			for (KVPair& kv : keys)
				if (kv.second == v)
					return &kv;

			KVPair* pair;
			if (left && (pair =		left->GetWithValue(v)))		return pair;
			if (right && (pair =	right->GetWithValue(v)))	return pair;
			return nullptr;
		}

		Node* Copy(const Hashmap& map)
		{
			Node* node = map->_NewNode(hash);
			node->keys = keys;

			if (left)	node->left =	left->Copy();
			if (right)	node->right =	right->Copy();

			return node;
		}

		void AddToK(Buffer<const K*>& buffer) const
		{
			for (const KVPair& kv : keys)
				buffer.Add(&kv.first);

			if (left)	left->AddToK(buffer);
			if (right)	right->AddToK(buffer);
		}

		void AddTo(Buffer<KVPair*> &buffer, int depth, int currentDepth = 0)
		{
			if (depth < 0 || currentDepth == depth)
				for (KVPair& kv : keys)
					buffer.Add(&kv);
			
			if (left)	left->AddTo(buffer, depth, currentDepth + 1);
			if (right)	right->AddTo(buffer, depth, currentDepth + 1);
		}

		void AddTo(Buffer<KVPairConst*>& buffer, int depth, int currentDepth = 0)
		{
			if (depth < 0 || currentDepth == depth)
				for (KVPair& kv : keys)
					buffer.Add(reinterpret_cast<KVPairConst*>(&kv));

			if (left)	left->AddTo(buffer, depth, currentDepth + 1);
			if (right)	right->AddTo(buffer, depth, currentDepth + 1);
		}

		template <typename Function>
		void ForEach(Function function)
		{
			for (KVPair& kv : keys)
				function((const K&)kv.first, (V&)kv.second);
			
			if (left)	left->ForEach(function);
			if (right)	right->ForEach(function);
		}
	};

	Node *_data;

public:
	Hashmap() : _data(nullptr) {}
	Hashmap(const NewHandler& newHandler, const DeleteHandler& deleteHandler) : _handlerNew(newHandler), _handlerDelete(deleteHandler), _data(nullptr) {}
	Hashmap(const Hashmap& other) : _data(nullptr) { if (other._data) _data = other._data->Copy(); }
	Hashmap(Hashmap&& other) : _data(other._data)
	{
		other._data = nullptr;

		if (other._handlerNew != _handlerNew || other._handlerDelete != _handlerDelete)
			operator=((const Hashmap&)*this);
	}

	~Hashmap() 
	{
		if (_data)
		{
			_data->DeleteChildren(*this);
			_DeleteNode(_data);
		}
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

	uint32 GetSize() const	{ return _data ? _data->Count() : 0; }
	bool IsEmpty() const	{ return _data == nullptr; }

	const K* const FindFirstKey(const V& value) const
	{
		if (_data)
		{
			KVPair* pair = _data->GetWithValue(value);
			if (pair) return &pair->first;
		}

		return nullptr;
	}

	const V* Get(const K& key) const
	{
		if (_data)
		{
			H_TYPE hash = _Hash(key);

			auto node = _data->Find(hash);
			if (node) return node->GetValueForKey(key);
		}

		return nullptr;
	}

	V& operator[](const K& key)
	{
		if (_data)
		{
			Node& node = _data->Get(_Hash(key), *this);

			V* value = node.GetValueForKey(key);
			if (value) return *value;

			return node.keys.Emplace(key)->second;
		}

		_data = _NewNode(_Hash(key));
		return _data->keys.Emplace(key)->second;
	}

	V& Set(const K& key, const V& value)
	{
		if (_data)
		{
			Node& node = _data->Get(_Hash(key), *this);
		
			V* v = node.GetValueForKey(key);
			if (v)
				return (*v = value);

			return node.keys.Emplace(key, value)->second;
		}

		_data = _NewNode(_Hash(key));
		return _data->keys.Emplace(key, value)->second;
	}

	V* Get(const K& key) { return const_cast<V*>(((const Hashmap*)this)->Get(key)); }

	Hashmap& operator=(const Hashmap& other)
	{
		Clear();
		if (other._data) _data = other._data->Copy();
		return *this;
	}

	Hashmap& operator=(Hashmap&& other)
	{
		_data = other._data;
		other._data = nullptr;

		if (other._handlerNew != _handlerNew || other._handlerDelete != _handlerDelete)
			return operator=((const Hashmap&)*this);

		return *this;
	}

	Buffer<const K*> ToKBuffer() const
	{
		Buffer<const K*> buffer;
		if (_data) _data->AddToK(buffer);
		return buffer;
	}

	Buffer<KVPair*> ToKVBuffer(int depth = -1)
	{
		Buffer<KVPair*> buffer;
		if (_data) _data->AddTo(buffer, depth);
		return buffer;
	}

	Buffer<KVPairConst*> ToKVBuffer() const
	{
		Buffer<KVPairConst*> buffer;
		if (_data) _data->AddTo(buffer, -1);
		return buffer;
	}

	//Function args must be (const K&, V&)
	template <typename Function>
	void ForEach(Function function)
	{
		if (_data) _data->ForEach(function);
	}
};
