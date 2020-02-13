#include "Bounds.hpp"
#include "Maths.hpp"

void Bounds::RecalculateSphereBounds()
{
	centre = (max + min) / 2.f;
	radius = Maths::SquareRoot(Maths::Max((min - centre).Abs().LengthSquared(), (max - centre).Abs().LengthSquared()));
}
