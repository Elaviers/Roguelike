#pragma once
#include <Engine/EngineInstance.hpp>
#include <Engine/EntCamera.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELSys/GLContext.hpp>
#include <ELSys/GLCubemap.hpp>
#include <ELSys/GLProgram.hpp>
#include <ELSys/InputManager.hpp> //For keycode
#include <ELSys/Timer.hpp>
#include <ELSys/Window.hpp>
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
	Window _window;

	GLContext _glContext;

	GLProgram _shaderUnlit;
	GLProgram _shaderLit;

	bool _consoleIsActive;
	bool _uiIsActive;

	UIContainer _ui;
	EntCamera _uiCamera;

	Entity _world;

	GLCubemap _reflect;

	void _InitGL();
	void _Init();
public:
	Game() : _consoleIsActive(false), _uiIsActive(true), _running(false), _deltaTime(0.f) {}
	~Game() {}

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
