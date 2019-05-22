#include "Bounds.hpp"
#include "Utilities.hpp"

void Bounds::RecalculateSphereBounds()
{
	centre = (max + min) / 2.f;
	radius = Maths::SquareRoot(Utilities::Max((min - centre).Abs().LengthSquared(), (max - centre).Abs().LengthSquared()));
}
