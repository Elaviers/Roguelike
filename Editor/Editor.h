#pragma once
#include <Engine/Camera.h>
#include <Engine/GLProgram.h>
#include <Engine/InputManager.h>
#include <Engine/ModelManager.h>
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
	GLProgram _basicShader;

	InputManager _inputManager;
	ModelManager _modelManager;
	TextureManager _textureManager;

	Viewport _viewports[2];
	Camera _cameras[2];

	int _mouseViewport;
	unsigned short _mouseX;
	unsigned short _mouseY;

	static LRESULT CALLBACK _WindowProc(HWND, UINT, WPARAM, LPARAM);

	void _Init();
	void _InitGL();

public:
	Editor();
	~Editor();

	void Run();
	void Frame();
	void Render();

	void Zoom(float);

	inline void UpdateMousePosition(int vpIndex, unsigned short x, unsigned short y)
	{
		_mouseViewport = vpIndex;
		_mouseX = x;
		_mouseY = y;
	}
};
