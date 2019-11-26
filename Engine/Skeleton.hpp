#pragma once
#include "List.hpp"
#include "Joint.hpp"

class Skeleton
{
private:
	int _nextJointID;

	List<Joint> _joints;
	Buffer<Mat4> _skinningMatrices;

	bool _CanAddJoint(const Joint *parent) const
	{
		if (parent == nullptr)
			return true;

		for (auto it = _joints.First(); it; ++it)
			if (&*it == parent)
				return true;

		return false;
	}

public:
	Skeleton() : _nextJointID(0) {}

	Skeleton(const Skeleton& other) : _nextJointID(other._nextJointID), _joints(other._joints), _skinningMatrices(other._skinningMatrices) {}

	Skeleton(Skeleton&& other) noexcept : _nextJointID(other._nextJointID), _joints(other._joints)
	{
		other._nextJointID = 0;
		other.Clear();
	}

	~Skeleton() {}

	Skeleton& operator=(const Skeleton& other)
	{
		_nextJointID = other._nextJointID;
		_joints = other._joints;
		_skinningMatrices = other._skinningMatrices;

		return *this;
	}

	Skeleton& operator=(Skeleton&& other) noexcept
	{
		_nextJointID = other._nextJointID;
		other._nextJointID = 0;

		_joints = std::move(other._joints);

		return *this;
	}

	void Clear() { _joints.Clear(); }

	size_t GetJointCount() const { return _joints.GetSize(); }

	List<Joint>::Iterator FirstListElement() { return _joints.First(); }
	const List<Joint>::Iterator FirstListElement() const { return _joints.First(); }

	Joint* CreateJoint(Joint *parent)
	{
		if (_CanAddJoint(parent))
		{
			return &*_joints.Add(Joint(_nextJointID++, parent));

			_skinningMatrices.SetSize(_joints.GetSize());
		}

		return nullptr;
	}

	Joint* GetJointWithID(int id)
	{
		auto it = _joints.Get(id);

		return it ? &*it : nullptr;
	}

	Joint* GetJointWithName(const String& name)
	{
		for (auto it = _joints.First(); it; ++it)
			if (it->name == name)
				return &*it;

		return nullptr;
	}

	void SetSkinningForJoint(int jointId, Mat4 skinning) { _skinningMatrices[jointId] = skinning; }

	void ToShader() const;
};
