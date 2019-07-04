#pragma once
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

		inline V* GetValueForKey(const K &key)
		{
			for (auto n = keys.First(); n; n = n->next)
				if (n->obj.first == key)
					return &n->obj.second;

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
			for (auto node = keys.First(); node; node = node->next)
				if (node->obj.second == v)
					return &node->obj;

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

		void AddTo(Buffer<List<KVPair>*> &buffer, int depth, int currentDepth = 0)
		{
			if (currentDepth == depth)
			{
				buffer.Add(&keys);
				return;
			}
			
			if (left)	left->AddTo(buffer, depth, currentDepth + 1);
			if (right)	right->AddTo(buffer, depth, currentDepth + 1);
		}
	};

	HashNode *_data;

public:
	Hashmap() : _data(nullptr) {}
	~Hashmap() { delete _data; }

	inline void Clear() { delete _data; }
	inline uint32 GetSize() const { return _data ? _data->Count() : 0; }
	inline bool IsEmpty() const { return _data == nullptr; }

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

			return node.keys.Add(KVPair(key, V()))->obj.second;
		}

		_data = new HashNode(_Hash(key));
		return _data->keys.Add(KVPair(key, V()))->obj.second;
	}

	V& Set(const K& key, const V& value)
	{
		if (_data)
		{
			HashNode& node = _data->Get(_Hash(key));
		
			V* v = node.GetValueForKey(key);
			if (v)
				return (*v = value);

			return node.keys.Add(KVPair(key, value))->obj.second;
		}

		_data = new HashNode(_Hash(key));
		_data->keys.Add(KVPair(key, value))->obj.second;
	}

	inline V* Get(const K& key) { return const_cast<V*>(((const Hashmap*)this)->Get(key)); }

	Buffer<List<KVPair>*> ToBuffer(int depth = -1)
	{
		Buffer<List<KVPair>*> buffer;
		if (_data) _data->AddTo(buffer, depth);
		return buffer;
	}
};
