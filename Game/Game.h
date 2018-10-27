#pragma once
#include <Engine/GLContext.h>
#include <Engine/GLProgram.h>
#include <Engine/InputManager.h>
#include <Engine/MaterialManager.h>
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

	GLContext _glContext;
	GLProgram _shader;

	InputManager _inputManager;
	MaterialManager _materialManager;
	ModelManager _modelManager;
	TextureManager _textureManager;

	static LRESULT CALLBACK _WindowProc(HWND, UINT, WPARAM, LPARAM);

	void _InitWindow();
	void _InitGL();
	void _Init();
public:
	Game();
	~Game();

	void Run();
	void Frame();
	void Render();
};
