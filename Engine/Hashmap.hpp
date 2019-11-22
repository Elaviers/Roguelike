#pragma once
#include "Buffer.hpp"
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
	static H_TYPE _Hash(const K& key) { return Hasher<H_TYPE>::template Hash<K>(key); }

	typedef Pair<const K, V> KVPair;
	typedef Pair<const K, const V> KVPairConst;

	class HashNode
	{
	public:
		const H_TYPE hash;
		List<KVPair> keys;
		
		HashNode *left, *right;

		HashNode(H_TYPE hash) : hash(hash), left(nullptr), right(nullptr) {}

		~HashNode()
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

		V* GetValueForKey(const K &key)
		{
			for (auto it = keys.First(); it; ++it)
				if (it->first == key)
					return &it->second;

			return nullptr;
		}

		HashNode* Find(H_TYPE h)
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

		HashNode& Get(H_TYPE h)
		{
			if (h < hash)
			{
				if (left) return left->Get(h);
				return *(left = new HashNode(h));
			}
			else if (h > hash)
			{
				if (right) return right->Get(h);
				return *(right = new HashNode(h));
			}

			return *this;
		}

		KVPair* GetWithValue(const V& v)
		{
			for (auto it = keys.First(); it; ++it)
				if (it->second == v)
					return &*it;

			KVPair* pair;
			if (left && (pair =		left->GetWithValue(v)))		return pair;
			if (right && (pair =	right->GetWithValue(v)))	return pair;
			return nullptr;
		}

		HashNode* Copy()
		{
			HashNode* node = new HashNode(hash);
			node->keys = keys;

			if (left)	node->left =	left->Copy();
			if (right)	node->right =	right->Copy();

			return node;
		}

		void AddToK(Buffer<const K*>& buffer) const
		{
			for (auto it = keys.First(); it; ++it)
				buffer.Add(&it->first);

			if (left)	left->AddToK(buffer);
			if (right)	right->AddToK(buffer);
		}

		void AddTo(Buffer<KVPair*> &buffer, int depth, int currentDepth = 0)
		{
			if (depth < 0 || currentDepth == depth)
				for (auto it = keys.First(); it; ++it)
					buffer.Add(&*it);
			
			if (left)	left->AddTo(buffer, depth, currentDepth + 1);
			if (right)	right->AddTo(buffer, depth, currentDepth + 1);
		}

		void AddTo(Buffer<KVPairConst*>& buffer, int depth, int currentDepth = 0)
		{
			if (depth < 0 || currentDepth == depth)
				for (auto it = keys.First(); it; ++it)
					buffer.Add(reinterpret_cast<KVPairConst*>(&*it));

			if (left)	left->AddTo(buffer, depth, currentDepth + 1);
			if (right)	right->AddTo(buffer, depth, currentDepth + 1);
		}

		void ForEach(void (*function)(const K&, V&))
		{
			for (auto it = keys.First(); it; ++it)
				function(it->first, it->second);
			
			if (left)	left->ForEach(function);
			if (right)	right->ForEach(function);
		}
	};

	HashNode *_data;

public:
	Hashmap() : _data(nullptr) {}
	Hashmap(const Hashmap& other) : _data(nullptr) { if (other._data) _data = other._data->Copy(); }
	~Hashmap() { delete _data; }

	void Clear() { delete _data; _data = nullptr; }
	uint32 GetSize() const { return _data ? _data->Count() : 0; }
	bool IsEmpty() const { return _data == nullptr; }

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
			HashNode& node = _data->Get(_Hash(key));

			V* value = node.GetValueForKey(key);
			if (value) return *value;

			return node.keys.Add(KVPair(key, V()))->second;
		}

		_data = new HashNode(_Hash(key));
		return _data->keys.Add(KVPair(key, V()))->second;
	}

	V& Set(const K& key, const V& value)
	{
		if (_data)
		{
			HashNode& node = _data->Get(_Hash(key));
		
			V* v = node.GetValueForKey(key);
			if (v)
				return (*v = value);

			return node.keys.Add(KVPair(key, value))->second;
		}

		_data = new HashNode(_Hash(key));
		return _data->keys.Add(KVPair(key, value))->second;
	}

	V* Get(const K& key) { return const_cast<V*>(((const Hashmap*)this)->Get(key)); }

	Hashmap& operator=(const Hashmap& other)
	{
		Clear();
		if (other._data) _data = other._data->Copy();
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

	void ForEach(void (*function)(const K&, V&))
	{
		if (_data) _data->ForEach(function);
	}
};
