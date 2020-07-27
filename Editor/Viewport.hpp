#pragma once
#include <Engine/EntCamera.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELUI/Rect.hpp>

struct Viewport
{
	EDirection gridPlane = EDirection::UP;

	EntCamera camera;

	UIRect ui;

	RenderQueue renderQueue;
};
