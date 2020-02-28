#pragma once
#include <Engine/GLContext.hpp>
#include <Engine/GLProgram.hpp>
#include <Engine/EntCamera.hpp>
#include <Engine/InputManager.hpp> //For keycode
#include <Engine/Timer.hpp>
#include <Engine/UIContainer.hpp>
#include <Engine/Window.hpp>
#include <Windows.h>

class Game
{
private:
	bool _running;
	float _deltaTime;

	Timer _timer;
	Window _window;

	GLContext _glContext;

	GLProgram _shaderUnlit;
	GLProgram _shaderLit;

	bool _consoleIsActive;
	bool _uiIsActive;

	UIContainer _ui;
	EntCamera _uiCamera;

	Entity _world;

	short _mouseXForFrame;
	short _mouseYForFrame;

	static LRESULT CALLBACK _WindowProc(HWND, UINT, WPARAM, LPARAM);

	void _InitWindow();
	void _InitGL();
	void _Init();
public:
	Game() : _consoleIsActive(false), _uiIsActive(true), _running(false), _deltaTime(0.f), _mouseXForFrame(0), _mouseYForFrame(0) {}
	~Game() {}

	void Run();
	void Frame();
	void Render();

	void StartLevel(const String &filename);

	void Resize(uint16 w, uint16 h);

	void MouseInput(short x, short y);
	void MouseMove(uint16 x, uint16 y);
	void MouseUp();
	void MouseDown();

	void KeyDown(EKeycode key);
	void KeyUp(EKeycode key);
	void InputChar(char character);

	void ButtonQuit();
};
