#pragma once
#include "GameObject.hpp"
#include "Types.hpp"
#include "Vector.hpp"

class ObjBox : public GameObject
{
	Vector3 _p1, _p2;

	void _UpdateMinMax();
	void _UpdateTransform();

protected:
	Vector3 _min, _max;

	Vector4 _colour;

public:
	GAMEOBJECT_FUNCS(ObjBox)

	ObjBox(byte flags = 0) : GameObject(flags), _colour(1.f, 1.f, 1.f, 1.f) {}
	virtual ~ObjBox() {}

	virtual void Render() const override;

	virtual Bounds GetBounds() const override { return Bounds(_min, _max); }

	inline void SetPoint1(const Vector3 &point1) 
	{
		_p1 = point1;
		_UpdateMinMax();
	}

	inline void SetPoint2(const Vector3 &point2)
	{
		_p2 = point2;
		_UpdateMinMax();
	}

	inline void SetMin(const Vector3 &min) { _min = min; }
	inline void SetMax(const Vector3 &max) { _max = max; }

	inline void SetRenderColour(const Vector4 &colour) { _colour = colour; }

	inline const Vector3& GetPoint1() const { return _p1; }
	inline const Vector3& GetPoint2() const { return _p2; }
	inline const Vector3& GetMin() const { return _min; }
	inline const Vector3& GetMax() const { return _max; }
};
