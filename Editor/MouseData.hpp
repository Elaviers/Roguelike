#pragma once
#include <ELCore/Types.hpp>

class Viewport;
class WorldObject;
class Geometry;

struct MouseData
{
	Viewport* viewport = nullptr;
	int x = 0;
	int y = 0;
	int prevX = 0;
	int prevY = 0;

	//Unit of the cursor on the viewport's grid
	float unitX = 0.f;
	float unitY = 0.f;
	float heldUnitX = 0.f;
	float heldUnitY = 0.f;
	float prevUnitX = 0.f;
	float prevUnitY = 0.f;

	float dragUnitX = 0.f;
	float dragUnitY = 0.f;

	bool isLeftDown = false;
	bool isRightDown = false;

	int unitX_rounded = 0;
	int unitY_rounded = 0;
	int heldUnitX_rounded = 0;
	int heldUnitY_rounded = 0;

	WorldObject* hoverObject = nullptr;
	uint32 hoverData[4];
};
