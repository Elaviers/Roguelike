#pragma once
#include <Engine/Camera.h>
#include <Engine/GLContext.h>
#include <Engine/GLProgram.h>
#include <Engine/FontManager.h>
#include <Engine/InputManager.h>
#include <Engine/MaterialManager.h>
#include <Engine/ModelManager.h>
#include <Engine/TextureManager.h>
#include <Engine/Timer.h>
#include <Engine/Window.h>
#include <Windows.h>
#include "MainMenu.h"

class Game
{
private:
	bool _running;
	float _deltaTime;

	Timer _timer;
	Window _window;

	GLContext _glContext;
	GLProgram _shader;

	FontManager _fontManager;
	InputManager _inputManager;
	MaterialManager _materialManager;
	ModelManager _modelManager;
	TextureManager _textureManager;

	MainMenu _mainMenu;
	Camera _uiCamera;

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

	void Resize(uint16 w, uint16 h);

	void MouseMove(unsigned short x, unsigned short y);
	void MouseDown();

	void ButtonStart();
	void ButtonQuit();
};
