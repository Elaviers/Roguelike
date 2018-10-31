#include "Box.h"
#include "DrawUtils.h"
#include "Engine.h"

void Box::Render() const
{
	if (Engine::modelManager)
	{
		Vector3 p1 = Vector3(point1[0], point1[1], point1[2]);
		Vector3 p2 = Vector3(point2[0], point2[1], point2[2]);

		DrawUtils::DrawBox(*Engine::modelManager, p1, p2);
	}

}
