#pragma once
#include "FunctionPointer.hpp"
#include <initializer_list>

template <typename T>
class List
{
	NewHandler _handlerNew;
	DeleteHandler _handlerDelete;

	class Node;

	template <typename... Args>
	Node* _NewNode(Node* next, Args... args) const
	{
		if (_handlerNew.IsCallable())
			return new (_handlerNew(sizeof(Node))) Node(next, args...);

		return new Node(next, args...);
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
		template<typename... Args>
		Node(Node* next, const Args&... args) : _value(args...), _next(next) {}
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
				dest = dest->_next = list._NewNode(nullptr, src->_value);
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
			return GetDeepestNode()->_next = list._NewNode(nullptr, value);
		}

		template<typename... Args>
		Node* Emplace(const List& list, Args... args)
		{
			return GetDeepestNode()->_next = list._NewNode(nullptr, args...);
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
			return node->_next = list._NewNode(next, value);
		}

		//Depth must be greater than 0
		template <typename ...Args>
		Node* InsertChild(size_t depth, const List& list, Args ...args)
		{
			Node* node = this;
			size_t prev = depth - 1;
			for (size_t i = 0; i < prev; ++i)
				if ((node = node->_next) == nullptr)
					return nullptr;

			Node* next = node->_next;
			return node->_next = list._NewNode(next, args...);
		}

		Node* InsertChildBefore(const T& value, const Node* before, const List& list)
		{
			Node* current = this;
			Node* next = _next;
			while (next)
			{
				if (next == before)
					return current->_next = list._NewNode(next->_next, value);

				current = next;
				next = current->_next;
			}

			return nullptr;
		}

		template <typename ...Args>
		Node* InsertChildBefore(const Node* before, const List& list, Args ...args)
		{
			Node* current = this;
			Node* next = _next;
			while (next)
			{
				if (next == before)
					return current->_next = list._NewNode(next->_next, args...);

				current = next;
				next = current->_next;
			}

			return nullptr;
		}

		//Returns node after the removed one
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
					return current->_next;
				}

				current = next;
				next = current->_next;
			}

			return nullptr;
		}

		//Returns node after the removed one
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
				return node->_next;
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

					if (current->_next == nullptr)
						return true;

					current = current->_next;
					next = current->_next;
				}
				else
				{
					current = next;
					next = current->_next;
				}
			}

			return success;
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
		Iterator() : _node(nullptr) {}
		Iterator(Node* node) : _node(node) {}

		bool IsValid() const		{ return _node != nullptr; }

		Iterator Next() const		{ return Iterator(_node->Next()); }
		Iterator& operator++()
		{
			if (_node) _node = _node->Next();
			return *this;
		}

		Iterator& operator=(const Iterator& other)
		{
			_node = other._node;
			return *this;
		}

		bool operator==(const Iterator& other) const { return _node == other._node; }
		bool operator!=(const Iterator& other) const { return _node != other._node; }

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

		bool operator==(const ConstIterator& other) const { return _node == other._node; }
		bool operator!=(const ConstIterator& other) const { return _node != other._node; }

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

	//Returns iterator pointing to next node
	Iterator _RemoveNode(const Node* node)
	{
		if (_first == nullptr) return Iterator(nullptr);
		if (_first == node)
		{
			if (_first == _last)
			{
				_DeleteNode(_first);
				_first = _last = nullptr;
				return Iterator(nullptr);
			}

			Node* next = _first->Next();
			_DeleteNode(_first);
			_first = next;
			return Iterator(_first);
		}

		Node* after = _first->RemoveChild(node, *this);
		
		if (after == nullptr)
			_last = _FindLast();

		return Iterator(after);
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
			_first = _NewNode(nullptr, other._first->Value());
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
			_first = _NewNode(nullptr, other._first->Value());
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

	//For range-based for
	Iterator begin()			{ return Iterator(_first); }
	ConstIterator begin() const { return ConstIterator(_first); }
	Iterator last()				{ return Iterator(_last); }
	ConstIterator last() const	{ return ConstIterator(_last); }
	Iterator end()				{ return Iterator(nullptr); }
	ConstIterator end() const	{ return ConstIterator(nullptr); }

	size_t GetSize() const		{ return _first ? _first->GetChildCount() + 1 : 0; }

	Iterator Add(const T& value)
	{
		if (_first) return Iterator(_last = _last->Add(value, *this));
		return Iterator(_first = _last = _NewNode(nullptr, value));
	}

	template <typename... Args>
	Iterator Emplace(Args... args)
	{
		if (_first) return Iterator(_last = _last->Emplace(*this, args...));
		return Iterator(_first = _last = _NewNode(nullptr, args...));
	}

	Iterator Insert(const T& value, Iterator before)
	{
		if (_first)
		{
			Node* node = before._node;

			if (node == _first) return Iterator(_first = _NewNode(_first, value));

			return Iterator(_first->InsertChildBefore(value, node, *this));
		}

		return Iterator(nullptr);
	}

	Iterator Insert(const T& value, size_t index)
	{
		if (index == 0)
		{
			Node* oldFirst = _first;
			return _first = _NewNode(oldFirst, value);
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

	template <typename ...Args>
	Iterator Insert(Iterator before, Args ...args)
	{
		if (_first)
		{
			Node* node = before._node;

			if (node == _first) return Iterator(_first = _NewNode(_first, args...));

			return Iterator(_first->InsertChildBefore(node, *this, args...));
		}

		return Iterator(nullptr);
	}

	template <typename ...Args>
	Iterator Insert(size_t index, Args ...args)
	{
		if (index == 0)
		{
			Node* oldFirst = _first;
			return _first = _NewNode(oldFirst, args...);
		}
		else if (_first)
		{
			Node* node = _first->InsertChild(index, *this, args...);
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

	//Returns iterator pointing to the element after the removed one
	Iterator Remove(const Iterator& iterator)		{ return _RemoveNode(iterator._node); }
	
	//Returns iterator pointing to the element after the removed one
	Iterator Remove(const ConstIterator& iterator)	{ return _RemoveNode(iterator._node); }

	//Returns iterator pointing to the element after the removed one
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

			Node* after = _first->RemoveChildAtDepth(index, *this);
			if (after == nullptr)
				_last = _FindLast();

			return Iterator(after);
		}

		return Iterator(nullptr);
	}

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
};
