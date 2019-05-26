#pragma once
#include "Quaternion.hpp"
#include "Vector.hpp"

class Rotation
{
	Quaternion _quaternion;
	Vector3 _euler;

	bool _quatBased = false;

	inline void _UpdateQuatFromEuler()
	{
		_quatBased = false;
		_quaternion = Quaternion(_euler);
	}

	inline void _UpdateEulerFromQuat()
	{
		_quatBased = true;
		_euler = _quaternion.ToEuler();
	}

public:
	Rotation() : _euler(), _quaternion() {}
	Rotation(const Vector3 &euler) : _euler(euler) { _UpdateQuatFromEuler(); }
	Rotation(const Quaternion& q) : _quaternion(q) { _UpdateEulerFromQuat(); }
	~Rotation() {}

	inline const Vector3& GetEuler() const
	{
		return _euler;
	}

	inline void SetEuler(const Vector3& euler)
	{
		_euler = euler;
		_UpdateQuatFromEuler();
	}

	inline void AddEuler(const Vector3& euler)
	{
		_euler += euler;
		_UpdateQuatFromEuler();
	}

	inline const Quaternion& GetQuat() const
	{
		return _quaternion;
	}

	inline void SetQuat(const Quaternion& q)
	{
		_quaternion = q;
		_UpdateEulerFromQuat();
	}

	inline Rotation& operator+=(const Rotation& other)
	{
		SetEuler(other.GetEuler() + _euler);

		return *this;
	}

	inline Rotation operator+(const Rotation& other) const
	{
		Rotation result = *this;
		return result += other;
	}

	inline Rotation operator*(const Rotation& other) const
	{
		Rotation result;
		result.SetQuat(_quaternion * other._quaternion);
		return result;
	}

	inline Rotation Inverse() const
	{
		return _quatBased ? Rotation(_quaternion.Inverse()) : Rotation(_euler * -1.f);
	}
};

