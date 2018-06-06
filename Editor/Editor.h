#pragma once
#include <Engine/GLProgram.h>
#include <Engine/InputManager.h>
#include <Engine/TextureManager.h>
#include <Engine/Timer.h>
#include <Engine/Window.h>
#include "Viewport.h"

class Editor
{
private:
	bool _running;
	float _deltaTime;

	Timer _timer;
	Window _window;

	GLProgram _shader;

	InputManager _inputManager;
	TextureManager _textureManager;

	Viewport _viewports[4];

	static LRESULT CALLBACK _WindowProc(HWND, UINT, WPARAM, LPARAM);

	void _Init();
	void _InitGL();

public:
	Editor();
	~Editor();

	void Run();
	void Frame();
	void Render();
};

