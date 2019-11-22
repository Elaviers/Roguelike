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

	int GetID() const { return _id; }
	Joint* GetParent() { return _parent; }
	const Joint* GetParent() const { return _parent; }
};
