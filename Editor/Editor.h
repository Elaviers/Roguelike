#pragma once
#include <Engine/Brush2D.h>
#include <Engine/Brush3D.h>
#include <Engine/Camera.h>
#include <Engine/GameObject.h>
#include <Engine/GLContext.h>
#include <Engine/GLProgram.h>
#include <Engine/InputManager.h>
#include <Engine/Level.h>
#include <Engine/MaterialManager.h>
#include <Engine/ModelManager.h>
#include <Engine/Registry.h>
#include <Engine/TextureManager.h>
#include <Engine/Timer.h>
#include <Engine/Window.h>
#include "PropertyWindow.h"
#include "ToolWindow.h"
#include "Viewport.h"

#include <CommCtrl.h>

#define VIEWPORTCOUNT 4

enum class Tool
{
	SELECT,
	EDIT,
	BRUSH,
	ENTITY,
	CONNECTOR
};

class Editor
{
private:
	bool _running;
	float _deltaTime;

	//Engine
	Registry _registry;
	Timer _timer;

	//Managers
	InputManager _inputManager;
	MaterialManager _materialManager;
	ModelManager _modelManager;
	TextureManager _textureManager;

	//Window stuff
	Window _window;
	Window _vpArea;
	PropertyWindow _propertyWindow;
	ToolWindow _toolWindow;

	HBRUSH _windowBrush;
	HWND _toolbar;
	HIMAGELIST _tbImages;

	Viewport _viewports[VIEWPORTCOUNT];
	Camera _cameras[VIEWPORTCOUNT];

	//OpenGL
	GLContext _glContext;
	GLProgram _shaderLit;
	GLProgram _shaderUnlit;

	//Editor
	Tool _tool;
	
	Level _level;

	struct
	{
		struct
		{
			Box box;
			Buffer<GameObject*> selectedObjects;
		} select;

		struct
		{
			Brush2D object;
			ObjectProperties properties;
		} brush2D;

		struct
		{
			Brush3D object;
			ObjectProperties properties;
		} brush3D;

		struct
		{
			Connector object;
			ObjectProperties properties;
		} connector;

		bool placing;
	} _toolData;

	struct
	{
		int viewport;
		int x;
		int y;
		float unitX;
		float unitY;
		int unitX_rounded;
		int unitY_rounded;

		bool isLeftDown;
		int heldUnitX_rounded;
		int heldUnitY_rounded;
	} _mouseData;

	float _axisMoveX;
	float _axisMoveY;
	float _axisLookX;
	float _axisLookY;

	static LRESULT CALLBACK _WindowProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK _vpAreaProc(HWND, UINT, WPARAM, LPARAM);

	void _Init();
	void _InitGL();

public:
	Editor();
	~Editor();

	void Run();
	void Frame();
	void Render();
	void RenderViewport(int index, Direction side);

	void Zoom(float);

	void UpdateMousePosition(int vpIndex, unsigned short x, unsigned short y);
	void LeftMouseDown();
	void LeftMouseUp();

	void Submit();

	void ResizeViews(uint16 w, uint16 h);

	void SetTool(Tool);

	String SelectMaterialDialog();
	String SelectModelDialog();
};
