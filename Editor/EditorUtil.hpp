#pragma once
#include <ELMaths/Vector2.hpp>

struct MouseData;

class World;
class WorldObject;

namespace EditorUtil
{

	void CalculatePointsFromMouseData(const MouseData&, Vector2 &out_p1, Vector2 &out_p2);

	//returns true if selection was changed
	bool WorldIMGUI(World&, Buffer<WorldObject*>& selectedObjects);
}
