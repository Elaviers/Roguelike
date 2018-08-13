#pragma once
#include <Engine/Camera.h>
#include <Engine/GameObject.h>
#include <Engine/GLContext.h>
#include <Engine/GLProgram.h>
#include <Engine/InputManager.h>
#include <Engine/MaterialManager.h>
#include <Engine/ModelManager.h>
#include <Engine/Registry.h>
#include <Engine/TextureManager.h>
#include <Engine/Timer.h>
#include <Engine/Window.h>
#include "Viewport.h"

#define VIEWPORTCOUNT 2

class Editor
{
private:
	bool _running;
	float _deltaTime;

	Timer _timer;
	Window _window;

	Registry _registry;

	GLContext _glContext;

	GLProgram _shader;
	GLProgram _basicShader;

	InputManager _inputManager;
	MaterialManager _materialManager;
	ModelManager _modelManager;
	TextureManager _textureManager;

	Viewport _viewports[VIEWPORTCOUNT];
	Camera _cameras[VIEWPORTCOUNT];

	int _mouseViewport;
	unsigned short _mouseX;
	unsigned short _mouseY;

	float _axisMoveX;
	float _axisMoveY;
	float _axisLookX;
	float _axisLookY;

	Buffer<GameObject*> _gameObjects;
	GameObject *_currentObject;

	static LRESULT CALLBACK _WindowProc(HWND, UINT, WPARAM, LPARAM);

	void _Init();
	void _InitGL();

	void _LoadModel(const char *filepath, const char *name);
	void _LoadTexture(const char *filepath, const char *name);

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
