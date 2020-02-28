#pragma once
#include <Engine/EntCamera.hpp>
#include <Engine/UIRect.hpp>

struct Viewport
{
	EDirection gridPlane = EDirection::UP;

	EntCamera camera;

	UIRect ui;
};
