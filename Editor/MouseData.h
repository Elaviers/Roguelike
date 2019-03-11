#pragma once
#include <Engine/Types.h>

struct MouseData
{
	int viewport = 0;
	int x = 0;
	int y = 0;
	float unitX = 0.f;
	float unitY = 0.f;
	int unitX_rounded = 0;
	int unitY_rounded = 0;

	byte forwardElement = 0;
	byte rightElement = 0;
	byte upElement = 0;

	bool isLeftDown = false;
	bool isRightDown = false;
	int heldUnitX_rounded = 0;
	int heldUnitY_rounded = 0;
};
