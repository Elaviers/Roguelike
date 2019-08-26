#pragma once
#include <Engine/Types.hpp>

struct MouseData
{
	int viewport = 0;
	int x = 0;
	int y = 0;

	float unitX = 0.f;
	float unitY = 0.f;
	float heldUnitX = 0.f;
	float heldUnitY = 0.f;
	float prevUnitX = 0.f;
	float prevUnitY = 0.f;

	bool isLeftDown = false;
	bool isRightDown = false;

	int unitX_rounded = 0;
	int unitY_rounded = 0;
	int heldUnitX_rounded = 0;
	int heldUnitY_rounded = 0;

	//ORTHO ONLY
	byte forwardElement = 0;
	byte rightElement = 0;
	byte upElement = 0;
};
