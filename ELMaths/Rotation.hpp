#pragma once
#include "Quaternion.hpp"
#include "Vector3.hpp"

class Rotation
{
	Quaternion _quaternion;
	Vector3 _euler;

	bool _quatBased = false;

	void _UpdateQuatFromEuler()
	{
		_quatBased = false;
		_quaternion = Quaternion(_euler);
	}

	void _UpdateEulerFromQuat()
	{
		_quatBased = true;
		_euler = _quaternion.ToEulerYXZ();
	}

public:
	Rotation() : _euler(), _quaternion() {}
	Rotation(const Vector3 &euler) : _euler(euler) { _UpdateQuatFromEuler(); }
	Rotation(const Quaternion& q) : _quaternion(q) { _UpdateEulerFromQuat(); }
	~Rotation() {}

	const Vector3& GetEuler() const
	{
		return _euler;
	}

	void SetEuler(const Vector3& euler)
	{
		_euler = euler;
		_UpdateQuatFromEuler();
	}

	void AddEuler(const Vector3& euler)
	{
		_euler += euler;
		_UpdateQuatFromEuler();
	}

	const Quaternion& GetQuat() const
	{
		return _quaternion;
	}

	void SetQuat(const Quaternion& q)
	{
		_quaternion = q;
		_UpdateEulerFromQuat();
	}

	Rotation& operator+=(const Rotation& other)
	{
		SetEuler(other.GetEuler() + _euler);

		return *this;
	}

	Rotation operator+(const Rotation& other) const
	{
		Rotation result = *this;
		return result += other;
	}

	Rotation operator*(const Rotation& other) const
	{
		Rotation result;
		result.SetQuat(_quaternion * other._quaternion);
		return result;
	}

	Rotation Inverse() const
	{
		return _quatBased ? Rotation(_quaternion.Inverse()) : Rotation(_euler * -1.f);
	}

	bool operator==(const Rotation& other) const
	{
		return _euler == other._euler;
	}
};

