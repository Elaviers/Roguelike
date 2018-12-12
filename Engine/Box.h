#pragma once
#include "Types.h"
#include "Vector.h"

class Box
{
	Vector3 _p1, _p2;

	void _UpdateMinMax();

protected:
	Vector3 _min, _max;

public:
	Box() {}
	virtual ~Box() {}

	virtual void Render() const;

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

	inline const Vector3& GetPoint1() const { return _p1; }
	inline const Vector3& GetPoint2() const { return _p2; }
	inline const Vector3& GetMin() const { return _min; }
	inline const Vector3& GetMax() const { return _max; }
};
