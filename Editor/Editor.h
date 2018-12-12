#pragma once
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
#include "MouseData.h"
#include "PropertyWindow.h"
#include "ToolBrush2D.h"
#include "ToolBrush3D.h"
#include "ToolConnector.h"
#include "ToolEntity.h"
#include "ToolSelect.h"
#include "ToolWindow.h"
#include "Viewport.h"
#include <CommCtrl.h>

#define VIEWPORTCOUNT 4


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

	//OpenGL
	GLContext _glContext;
	GLProgram _shaderLit;
	GLProgram _shaderUnlit;

	//Editor
	Level _level;

	Tool *_currentTool;

	struct _EditorTools
	{
		ToolSelect select;
		ToolBrush2D brush2D;
		ToolBrush3D brush3D;
		ToolEntity entity;
		ToolConnector connector;

		_EditorTools(Editor &editor) : select(editor), brush2D(editor), brush3D(editor), entity(editor), connector(editor) {}
	} _tools;


	MouseData _mouseData;

	float _axisMoveX;
	float _axisMoveY;
	float _axisLookX;
	float _axisLookY;

	static LRESULT CALLBACK _WindowProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK _vpAreaProc(HWND, UINT, WPARAM, LPARAM);

	void _Init();
	void _InitGL();

public:
	Editor() : _propertyWindow(this), _toolWindow(this), _tools(*this) {}
	~Editor() {}

	void Run();
	void Frame();
	void Render();
	void RenderViewport(int index, Direction side);

	void Zoom(float);

	void UpdateMousePosition(int vpIndex, unsigned short x, unsigned short y);
	void LeftMouseDown();
	void LeftMouseUp();

	void KeySubmit();
	void KeyCancel();
	void KeyDelete();

	void ResizeViews(uint16 w, uint16 h);

	String SelectMaterialDialog();
	String SelectModelDialog();

	//For Tools
	inline Level& LevelRef() { return _level; }
	inline Viewport& ViewportRef(int id) { return _viewports[id]; }
	inline Camera& CameraRef(int id) { return _viewports[id].CameraRef(); }
	inline PropertyWindow& PropertyWindowRef() { return _propertyWindow; }
};
