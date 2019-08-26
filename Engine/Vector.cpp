#include "Vector.hpp"
#include "Matrix.hpp"
#include "Rotation.hpp"
#include <cmath>

using namespace Maths;

namespace VectorMaths
{
	const Vector3 V3X(1, 0, 0);
	const Vector3 V3Y(0, 1, 0);
	const Vector3 V3Z(0, 0, 1);

	Vector3 GetPerpendicularVector(const Vector3& v)
	{
		static const Vector3 axes[3] = { Vector3(1,0,0), Vector3(0,1,0), Vector3(0,0,1) };

		float smallestDot = Vector3::Dot(axes[0], v);
		int axis = 0;
		for (int i = 1; i < 3; ++i)
		{
			float dot = Vector3::Dot(axes[i], v);
			if (dot < smallestDot)
			{
				smallestDot = dot;
				axis = i;
			}
		}

		return (axes[axis] - smallestDot * v).Normalise();
	}

	Vector3 Rotate(const Vector3 &vector, const Rotation &rotation)
	{
		return vector * rotation.GetQuat().ToMatrix();
	}
}
