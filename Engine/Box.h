#pragma once
#include "Types.h"
#include "Vector.h"

class Box
{
public:
	Box() {}
	virtual ~Box() {}

	Vector<int16, 3> point1;
	Vector<int16, 3> point2;

	void Render() const;
};

