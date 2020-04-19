#pragma once
#include <Engine/Vector2.hpp>

struct MouseData;

namespace EditorUtil
{

	void CalculatePointsFromMouseData(const MouseData&, Vector2 &out_p1, Vector2 &out_p2);

}
