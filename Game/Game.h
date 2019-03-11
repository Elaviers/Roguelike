#pragma once
#include <Engine/GLContext.h>
#include <Engine/GLProgram.h>
#include <Engine/ObjCamera.h>
#include <Engine/InputManager.h> //For keycode
#include <Engine/Timer.h>
#include <Engine/UIContainer.h>
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

	bool _consoleIsActive;

	UIContainer _ui;
	ObjCamera _uiCamera;

	static LRESULT CALLBACK _WindowProc(HWND, UINT, WPARAM, LPARAM);

	void _InitWindow();
	void _InitGL();
	void _Init();
public:
	Game() : _running(false), _deltaTime(0.f) {}
	~Game() {}

	void Run();
	void Frame();
	void Render();

	void StartLevel(const String &level);

	void Resize(uint16 w, uint16 h);

	void MouseMove(unsigned short x, unsigned short y);
	void MouseDown();

	void KeyDown(Keycode key);
	void InputChar(char character);

	void ButtonQuit();
};
