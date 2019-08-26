#pragma once
#include "List.hpp"
#include "Joint.hpp"

class Skeleton
{
private:
	int _nextJointID;

	List<Joint> _joints;

	Buffer<Mat4> _transformCache;

	bool _CanAddJoint(const Joint *parent) const
	{
		if (parent == nullptr)
		{
			for (auto it = _joints.First(); it; ++it)
				if (it->GetParent() == nullptr)
					return false;

			return true;
		}
		else
		{
			for (auto it = _joints.First(); it; ++it)
				if (&*it == parent)
					return true;

			return false;
		}
	}

public:
	Skeleton() : _nextJointID(0) {}

	Skeleton(Skeleton&& other) noexcept : _nextJointID(other._nextJointID), _joints(other._joints)
	{
		other._nextJointID = 0;
	}

	~Skeleton() {}

	inline Skeleton& operator=(Skeleton&& other) noexcept
	{
		_nextJointID = other._nextJointID;
		other._nextJointID = 0;

		_joints = std::move(other._joints);

		return *this;
	}

	inline void Clear() { _joints.Clear(); }

	inline size_t GetJointCount() const { return _joints.GetSize(); }

	inline List<Joint>::Iterator FirstListElement() { return _joints.First(); }
	inline const List<Joint>::Iterator FirstListElement() const { return _joints.First(); }

	Joint* CreateJoint(Joint *parent)
	{
		if (_CanAddJoint(parent))
			return &*_joints.Add(Joint(_nextJointID++, parent));

		if (_transformCache.GetSize() != 0)
			UpdateCache();

		return nullptr;
	}

	inline Joint* GetJointWithID(int id)
	{
		auto it = _joints.Get(id);

		return it ? &*it : nullptr;
	}

	//Updates the internal cache of joints to be sent to the shader
	void UpdateCache();

	void ToShader() const;
};
