#pragma once
#include <Engine/Types.h>

struct MouseData
{
	int viewport;
	int x;
	int y;
	float unitX;
	float unitY;
	int unitX_rounded;
	int unitY_rounded;

	byte forwardElement;
	byte rightElement;
	byte upElement;

	bool isLeftDown;
	bool isRightDown;
	int heldUnitX_rounded;
	int heldUnitY_rounded;
};
