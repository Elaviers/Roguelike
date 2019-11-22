#pragma once

template <typename T>
class List
{
	struct _Node
	{
		T obj;

		_Node* next;

		_Node() : obj(), next(nullptr) {}
		_Node(const T& element) : obj(element), next(nullptr) {}

		void DeleteAll()
		{
			if (next)
				next->DeleteAll();

			delete this;
			return;
		}

		size_t Count()
		{
			int i = 1;
			for (auto node = next; node; node = node->next)
				++i;

			return i;
		}

		_Node* Last()
		{
			if (next) return next->Last();
			return this;
		}

		const _Node* Last() const
		{
			if (next) return next->Last();
			return this;
		}

		static _Node* CopyOf(const _Node* from)
		{
			if (from == nullptr) return nullptr;

			_Node* result = new _Node(*from);
			_Node* to = result;
			
			while (from->next)
			{
				from = from->next;
				to = to->next = new _Node(*from);
			}

			return result;
		}
	};

	_Node* _first;

public:
	class Iterator
	{
		_Node* _node;

		friend class List;

	public:
		Iterator(_Node* node) : _node(node) {}
		~Iterator() {}

		Iterator Next() const { return _node->next; }

		Iterator& operator++() 
		{ 
			_node = _node->next;
			return *this;
		}

		T* operator->() const	{ return &_node->obj; }
		T& operator*() const		{ return _node->obj; }

		operator bool() const	{ return _node != nullptr; }
		bool IsValid() const		{ return _node != nullptr; }
	};

	List() : _first(nullptr) {}
	List(const List& other) : _first(_Node::CopyOf(other._first)) {}
	List(List&& other) : _first(other._first) { other._first = nullptr; }
	
	~List() { if (_first) _first->DeleteAll(); }

	List& operator=(const List& other)
	{
		Clear();
		_first = _Node::CopyOf(other._first);
		return *this;
	}

	List& operator=(List&& other) noexcept
	{
		_first = other._first;
		other._first = nullptr;

		return *this;
	}

	List& operator+=(const T& element)
	{
		Add(element);
		return *this;
	}

	void Clear() 
	{ 
		if (_first)
		{
			_first->DeleteAll();
			_first = nullptr;
		}
	}

	size_t GetSize() const { return _first ? _first->Count() : 0; }
	bool IsEmpty() const { return _first == nullptr; }

	Iterator First() { return _first; }
	Iterator First() const { return _first; }
	Iterator Last() { return _first ? _first->Last() : nullptr; }
	Iterator Last() const { return _first ? _first->Last() : nullptr; }

	Iterator Get(int index)
	{
		auto node = _first;
		for (int i = 0; node; node = node->next)
			if (i++ == index)
				return node;

		return nullptr;
	}

	Iterator Add(const T& element)
	{
		_Node* node = new _Node(element);
		
		if (_first)	_first->Last()->next = node;
		else		_first = node;

		return node;
	}

	void Remove(const T& element)
	{
		_Node* prev = nullptr;

		for (auto node = _first; node;)
		{
			if (node->obj == element)
			{
				auto next = node->next;

				if (prev)	prev->next = next;
				else		_first = next;

				delete node;
				node = next;
			}
			else
				node = node->next;

			prev = node;
		}
	}

	void Remove(const Iterator &target)
	{
		_Node* prev = nullptr;

		for (_Node* node = _first; node; node = node->next)
		{
			if (node == target._node)
			{
				if (prev)
					prev->next = node->next;
				else
					_first = node->next;

				delete node;
				return;
			}

			prev = node;
		}
	}
};
