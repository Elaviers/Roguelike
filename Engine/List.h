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

	~List() { Clear(); }

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

	inline Node* Last()
	{
		if (_first == nullptr)
			return nullptr;

		Node* node = _first;

		while (node->next)
			node = node->next;

		return node;
	}

	inline const Node* Last() const { return Last(); }

	Node* Add(const T& element)
	{
		Node* node = new Node(element);
		
		if (_first)
			Last()->next = node;
		else
			_first = node;

		return node;
	}

	inline Node* New() { return Add(T()); }

	void Remove(Node* target)
	{
		for (Node* node = _first; node; node = node->next)
			if (node->next == target)
			{
				node->next = target->next;
				delete target;
				return;
			}
	}

};
