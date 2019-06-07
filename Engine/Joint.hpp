#pragma once
#include "String.hpp"
#include "Transform.hpp"

class Joint
{
	int _id;
	Joint* _parent;

public:
	Joint(int id, Joint *parent = nullptr) : _id(id), _parent(parent) {}
	~Joint() {}

	String name;
	Transform localTransform;

	inline int GetID() const { return _id; }
	inline Joint* GetParent() { return _parent; }
	inline const Joint* GetParent() const { return _parent; }
};
