#pragma once

template <typename T>
class List
{
public:
	struct Node
	{
		T obj;

		Node* next;

		Node() : obj(), next(nullptr) {}
		Node(const T& element) : obj(element), next(nullptr) {}
	};

private:
	Node* _first;

public:
	List() : _first(nullptr) {}

	List(const List& other) : _first(nullptr)
	{
		if (other._first)
		{
			_first = new Node(other._first->obj);

			Node* destNode = _first;
			Node* srcNode = _first->next;
			while (srcNode)
			{
				destNode->next = new Node(srcNode->obj);
				destNode = destNode->next;
				
				srcNode = srcNode->next;
			}
		}
	}

	List(List&& other) : _first(other._first)
	{
		other._first = nullptr;
	}

	~List() { Clear(); }

	inline List& operator=(List&& other) noexcept
	{
		_first = other._first;
		other._first = nullptr;

		return *this;
	}

	inline Node* Get(int index)
	{
		if (_first == nullptr)
			return nullptr;

		auto node = _first;

		for (int i = 0; node; node = node->next)
			if (i++ == index)
				return node;

		return nullptr;
	}

	void Clear()
	{
		while (_first)
		{
			Node* next = _first->next;
			delete _first;
			_first = next;
		}
	}

	inline Node* First() { return _first; }
	inline const Node* First() const { return _first; }

	size_t GetSize() const
	{
		if (_first == nullptr)
			return 0;

		Node* node = _first;
		size_t count = 1;

		while (node = node->next)
			++count;

		return count;
	}

	Node* Last()
	{
		if (_first == nullptr)
			return nullptr;

		Node* node = _first;

		while (node->next)
			node = node->next;

		return node;
	}

	inline const Node* Last() const { return const_cast<List&>(this)->Last(); }

	Node* Add(const T& element)
	{
		Node* node = new Node(element);
		
		if (_first)
			Last()->next = node;
		else
			_first = node;

		return node;
	}

	void Remove(Node* target)
	{
		for (Node* node = _first; node; node = node->next)
			if (node == target)
			{
				if (node == _first)
					_first = target->next;
				else
					node->next = target->next;
				
				delete target;
				return;
			}
	}

};
