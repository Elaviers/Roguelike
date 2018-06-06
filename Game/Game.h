#pragma once
#include <Engine/GLProgram.h>
#include <Engine/InputManager.h>
#include <Engine/ModelManager.h>
#include <Engine/TextureManager.h>
#include <Engine/Timer.h>
#include <Engine/Window.h>
#include <Windows.h>

class Game
{
private:
	bool _running;
	float _deltaTime;

	Timer _timer;
	Window _window;

	GLProgram _shader;

	InputManager _inputManager;
	ModelManager _modelManager;
	TextureManager _textureManager;

	static LRESULT CALLBACK _WindowProc(HWND, UINT, WPARAM, LPARAM);

	void _Init();
	void _InitGL();
public:
	Game();
	~Game();

	void Run();
	void Frame();
	void Render();
};
