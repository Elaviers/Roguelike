#pragma once
#include "FunctionPointer.hpp"
#include <initializer_list>

template <typename T>
class List
{
	NewHandler _handlerNew;
	DeleteHandler _handlerDelete;

	class Node;

	Node* _NewNode(const T& value, Node* next) const
	{
		if (_handlerNew.IsCallable())
			return new (_handlerNew(sizeof(Node))) Node(value, next);

		return new Node(value, next);
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

	class Node
	{
		T _value;
		Node* _next;

	public:
		Node(const T& value, Node* next = nullptr) : _value(value), _next(next) {}
		~Node() {}

		T& Value()					{ return _value; }
		const T& Value() const		{ return _value; }

		Node* Next()				{ return _next; }
		const Node* Next() const	{ return _next; }

		void CopyChildrenFrom(const Node* other, const List& list)
		{
			Node* src = other->_next;
			Node* dest = GetDeepestNode();
			while (src)
			{
				dest = dest->_next = list._NewNode(src->_value, nullptr);
				src = src->_next;
			}
		}

		void DeleteChildren(const List& list)
		{
			Node* next = _next;
			while (next)
			{
				Node* after = next->_next;
				list._DeleteNode(next);
				next = after;
			}
		}

		bool ContainsValue(const T& value) const
		{
			for (const Node* node = this; node; node = node->_next)
				if (node->_value == value)
					return true;

			return false;
		}

		bool ContainsNode(const Node* n) const
		{
			for (const Node* node = this; node; node = node->_next)
				if (node == n)
					return true;

			return false;
		}

		size_t GetChildCount() const
		{
			size_t count = 0;
			const Node* node = this;
			while (node = node->_next)
				++count;

			return count;
		}

		Node* GetNodeAtDepth(size_t depth)
		{
			Node* node = this;
			for (size_t i = 0; i < depth; ++i)
				if ((node = node->_next) == nullptr)
					return nullptr;

			return node;
		}

		Node* GetDeepestNode()
		{
			Node* node = this;
			while (node->_next) node = node->_next;
			return node;
		}

		Node* Add(const T& value, const List& list)
		{
			return GetDeepestNode()->_next = list._NewNode(value, nullptr);
		}

		//Depth must be greater than 0
		Node* InsertChild(const T& value, size_t depth, const List& list)
		{
			Node* node = this;
			size_t prev = depth - 1;
			for (size_t i = 0; i < prev; ++i)
				if ((node = node->_next) == nullptr)
					return nullptr;

			Node* next = node->_next;
			return node->_next = list._NewNode(value, next);
		}

		Node* InsertChildBefore(const T& value, const Node* before, const List& list)
		{
			Node* current = this;
			Node* next = _next;
			while (next)
			{
				if (next == before)
					return current->_next = list._NewNode(value, next->_next);

				current = next;
				next = current->_next;
			}

			return nullptr;
		}

		//Returns node before the removed one
		Node* RemoveChild(const Node* node, const List& list)
		{
			Node* current = this;
			Node* next = _next;
			while (next)
			{
				if (next == node)
				{
					current->_next = next->_next;
					list._DeleteNode(next);
					return current;
				}

				current = next;
				next = current->_next;
			}

			return nullptr;
		}

		bool RemoveChildrenWithValue(const T& value, const List& list)
		{
			bool success = false;
			Node* current = this;
			Node* next = _next;
			while (next)
			{
				if (next->_value == value)
				{
					current->_next = next->_next;
					list._DeleteNode(next);
					success = true;
				}

				current = next;
				next = current->_next;
			}

			return success;
		}

		//Returns node before the removed one
		Node* RemoveChildAtDepth(size_t depth, const List& list)
		{
			Node* node = this;
			size_t prev = depth - 1;
			for (size_t i = 0; i < prev; ++i)
				if ((node = node->_next) == nullptr)
					return nullptr;

			if (node->_next)
			{
				Node* next = node->_next->_next;
				list._DeleteNode(node->_next);
				node->_next = next;
				return node;
			}

			return nullptr;
		}

		const Node* GetNodeBeforeChild(const Node* node) const	{ return (const Node*)(const_cast<Node*>(this)->GetNodeBeforeChild(node)); }
		const Node* GetNodeAtDepth(size_t depth) const			{ return (const Node*)(const_cast<Node*>(this)->GetNodeAtDepth(depth)); }
		const Node* GetDeepestNode() const						{ return (const Node*)(const_cast<Node*>(this)->GetDeepestNode()); }
	};

public:
	class Iterator
	{
		friend List;
		Node* _node;

	public:
		Iterator(Node* node) : _node(node) {}

		bool IsValid() const		{ return _node != nullptr; }

		Iterator Next() const		{ return Iterator(_node->Next()); }
		Iterator& operator++()
		{
			if (_node) _node = _node->Next();
			return *this;
		}

		Iterator& operator+=(size_t offset)
		{
			for (int i = 0; i < offset; ++i)
				operator++();

			return *this;
		}

		Iterator operator+(size_t offset)
		{
			return Iterator(*this) += offset;
		}

		T* operator->()				{ return &_node->Value(); }
		T& operator*()				{ return _node->Value(); }
		const T* operator->() const { return &_node->Value(); }
		const T& operator*() const	{ return _node->Value(); }
	};

	class ConstIterator
	{
		friend List;
		const Node* _node;

	public:
		ConstIterator(const Node* node) : _node(node) {}

		bool IsValid() const		{ return _node != nullptr; }

		ConstIterator Next() const	{ return ConstIterator(_node->Next()); }
		ConstIterator& operator++()
		{
			if (_node) _node = _node->Next();
			return *this;
		}

		ConstIterator& operator+=(size_t offset)
		{
			for (int i = 0; i < offset; ++i)
				operator++();

			return *this;
		}

		ConstIterator operator+(size_t offset)
		{
			return ConstIterator(*this) += offset;
		}

		const T* operator->() const { return &_node->Value(); }
		const T& operator*() const	{ return _node->Value(); }
	};

private:
	Node* _first;
	Node* _last;

	Node* _FindLast() { return _first ? _first->GetDeepestNode() : nullptr; }

	Iterator _RemoveNode(const Node* node)
	{
		if (_first == nullptr) return Iterator(nullptr);
		if (_first == node)
		{
			if (_first == _last)
			{
				_DeleteNode(_first);
				_first = _last = nullptr;
				return Iterator(_first);
			}

			Node* next = _first->Next();
			_DeleteNode(_first);
			_first = next;
			return Iterator(_first);
		}

		Node* before = _first->RemoveChild(node, *this);
		if (before)
		{
			if (_last == node)
				_last = _FindLast();

			return Iterator(before);
		}

		return Iterator(nullptr);
	}

public:
	List() : _first(nullptr), _last(nullptr) {}
	List(const NewHandler& newHandler, const DeleteHandler& deleteHandler) : _first(nullptr), _last(nullptr), _handlerNew(newHandler), _handlerDelete(deleteHandler) {}

	List(const std::initializer_list<T>& elements)
	{
		_first = nullptr;

		if (elements.size() > 0)
			for (size_t i = 0; i < elements.size(); ++i)
				Add(elements.begin()[i]);
		else
			_last = nullptr;
	}

	List(const NewHandler& newHandler, const DeleteHandler& deleteHandler, const std::initializer_list<T>& elements) : _handlerNew(newHandler), _handlerDelete(deleteHandler)
	{
		List(elements);
	}

	List(const List& other) : _first(nullptr), _last(nullptr)
	{
		if (other._first)
		{
			_first = _NewNode(other._first->Value(), nullptr);
			_first->CopyChildrenFrom(other._first, *this);
			_last = _first->GetDeepestNode();
		}
	}

	List(List&& other) : _first(other._first), _last(other._last)
	{
		other._first = other._last = nullptr;
	}

	~List()
	{
		if (_first)
		{
			_first->DeleteChildren(*this);
			_DeleteNode(_first);
		}
	}

	void Clear()
	{
		if (_first)
		{
			_first->DeleteChildren(*this);
			_DeleteNode(_first);
			_first = _last = nullptr;
		}
	}

	List& operator=(const List& other)
	{
		Clear();

		if (other._first)
		{
			_first = _NewNode(other._first->Value(), nullptr);
			_first->CopyChildrenFrom(other._first, *this);
			_last = _first->GetDeepestNode();
		}
		else
			_first = _last = nullptr;

		return *this;
	}

	List& operator=(List&& other) noexcept
	{
		Clear();

		_first = other._first;
		_last = other._last;
		other._first = other._last = nullptr;

		if (_handlerNew != other._handlerNew || _handlerDelete != other._handlerDelete)
			return operator=((const List&)*this);

		return *this;
	}

	Iterator First()			{ return Iterator(_first); }
	ConstIterator First() const { return ConstIterator(_first); }

	Iterator Last()				{ return Iterator(_last); }
	ConstIterator Last() const	{ return ConstIterator(_last); }

	size_t GetSize() const		{ return _first ? _first->GetChildCount() + 1 : 0; }

	Iterator Add(const T& value)
	{
		if (_first) return Iterator(_last = _first->Add(value, *this));
		return Iterator(_first = _last = _NewNode(value, nullptr));
	}

	Iterator Insert(const T& value, Iterator before)
	{
		if (_first)
		{
			Node* node = before._node;

			if (node == _first) return Iterator(_first = _NewNode(value, _first));

			return Iterator(_first->InsertChildBefore(value, node, *this));
		}

		return Iterator(nullptr);
	}

	Iterator Insert(const T& value, size_t index)
	{
		if (index == 0)
		{
			Node* oldFirst = _first;
			return _first = _NewNode(value, oldFirst);
		}
		else if (_first)
		{
			Node* node = _first->InsertChild(value, index, *this);
			if (node->Next() == nullptr)
				_last = node;

			return node;
		}

		return Iterator(nullptr);
	}

	Iterator Get(size_t index)
	{
		if (_first) return Iterator(_first->GetNodeAtDepth(index));
		return Iterator(nullptr);
	}

	ConstIterator Get(size_t index) const
	{
		if (_first) return ConstIterator(_first->GetNodeAtDepth(index));
		return ConstIterator(nullptr);
	}

	bool Contains(const T& value) const
	{
		return _first ? _first->ContainsValue(value) : false;
	}

	bool Contains(const Iterator& iterator) const		{ return _first ? _first->ContainsNode(iterator._node) : false; }
	bool Contains(const ConstIterator& iterator) const	{ return _first ? _first->ContainsNode(iterator._node) : false; }

	Iterator Remove(const Iterator& iterator)		{ return _RemoveNode(iterator._node); }
	Iterator Remove(const ConstIterator& iterator)	{ return _RemoveNode(iterator._node); }

	bool Remove(const T& value)
	{
		if (_first == nullptr) return false;
		while (_first->Value() == value)
		{
			Node* next = _first->Next();
			_DeleteNode(_first);
			_first = next;

			if (_first == nullptr)
			{
				_last = nullptr;
				return true;
			}
		}

		bool success = _first->RemoveChildrenWithValue(value, *this);
		_last = _FindLast();
		return success;
	}

	Iterator RemoveIndex(size_t index)
	{
		if (_first)
		{
			if (index == 0)
			{
				Node* next = _first->Next();
				_DeleteNode(_first);
				_first = next;

				if (_first == nullptr) _last = nullptr;

				return Iterator(_first);
			}

			Node* before = _first->RemoveChildAtDepth(index, *this);
			if (before->Next() == nullptr)
				_last = before;

			return Iterator(before);
		}

		return Iterator(nullptr);
	}
};
