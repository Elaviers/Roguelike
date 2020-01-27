#pragma once
#include "Vector.hpp"

struct LineSegment
{
	Vector3 start;
	Vector3 end;

	LineSegment(const Vector3 &start, const Vector3 &end) : start(start), end(end) {}
};
