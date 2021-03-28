#include "EditorUtil.hpp"
#include "MouseData.hpp"
#include <Engine/World.hpp>
#include <Engine/WorldObject.hpp>
#include <Engine/imgui/imgui.h>

void EditorUtil::CalculatePointsFromMouseData(const MouseData &mouseData, Vector2 &out_p1, Vector2 &out_p2)
{
	if (mouseData.unitX_rounded <= mouseData.heldUnitX_rounded)
	{
		out_p1[0] = (float)(mouseData.heldUnitX_rounded + 1);
		out_p2[0] = (float)mouseData.unitX_rounded;
	}
	else
	{
		out_p1[0] = (float)mouseData.heldUnitX_rounded;
		out_p2[0] = (float)(mouseData.unitX_rounded + 1);
	}

	if (mouseData.unitY_rounded <= mouseData.heldUnitY_rounded)
	{
		out_p1[1] = (float)(mouseData.heldUnitY_rounded + 1);
		out_p2[1] = (float)mouseData.unitY_rounded;
	}
	else
	{
		out_p1[1] = (float)mouseData.heldUnitY_rounded;
		out_p2[1] = (float)(mouseData.unitY_rounded + 1);
	}
}
