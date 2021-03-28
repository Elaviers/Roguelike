#pragma once
#include <Engine/EngineInstance.hpp>
#include <Engine/EngineUtilities.hpp>
#include <Engine/World.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELMaths/Projection.hpp>
#include <ELSys/GLContext.hpp>
#include <ELSys/GLCubemap.hpp>
#include <ELSys/GLProgram.hpp>
#include <ELSys/InputManager.hpp> //For keycode
#include <ELSys/Timer.hpp>
#include <ELSys/Window_Win32.hpp>
#include <ELUI/Container.hpp>

class Game
{
private:
	EngineInstance _engine;
	RenderQueue _renderQueue;
	RenderQueue _uiQueue;

	bool _running;
	float _deltaTime;

	Timer _timer;
	Window_Win32 _window;

	GLContext _glContext;

	GLProgram _shaderUnlit;
	GLProgram _shaderLit;

	bool _consoleIsActive;
	bool _uiIsActive;

	UIContainer _ui;
	Projection _uiCamera;
	
	World _world;

	GLCubemap _reflect;

	bool _worldDebug;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void _InitGL();
	void _Init();
public:
	Game() : _consoleIsActive(false), _uiIsActive(true), _running(false), _deltaTime(0.f), _world(&_engine.context), _worldDebug(false) {}
	~Game();

	const Context& GetContext() const { return _engine.context; }

	void Run();
	void Frame();
	void Render();

	void StartLevel(const String &filename);

	void Resize(uint16 w, uint16 h);

	void MouseMove(uint16 x, uint16 y);
	void MouseUp();
	void MouseDown();

	void KeyDown(EKeycode key);
	void KeyUp(EKeycode key);
	void InputChar(char character);

	void ButtonQuit();
};
