#pragma once
#include <Engine/GLContext.h>
#include <Engine/GLProgram.h>
#include <Engine/LevelIO.h>
#include <Engine/Registry.h>
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
#include <fbxsdk.h>

#define VIEWPORTCOUNT 4


class Editor
{
private:
	bool _running = false;
	float _deltaTime = 0.f;

	//Engine
	Registry _registry;
	Timer _timer;

	FbxManager *_fbxManager;

	//Window stuff
	Window _window;
	Window _vpArea;
	PropertyWindow _propertyWindow;
	ToolWindow _toolWindow;

	HBRUSH _windowBrush = NULL;
	HWND _toolbar = NULL;
	HIMAGELIST _tbImages = NULL;

	ObjCamera _uiCam;

	Viewport _viewports[VIEWPORTCOUNT];

	//OpenGL
	GLContext _glContext;
	GLProgram _shaderLit;
	GLProgram _shaderUnlit;

	//Editor
	GameObject _level;

	Tool *_currentTool = nullptr;

	int _activeVP = 0;

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

	float _axisMoveX = 0.f;
	float _axisMoveY = 0.f;
	float _axisMoveZ = 0.f;
	float _axisLookX = 0.f;
	float _axisLookY = 0.f;

	static LRESULT CALLBACK _WindowProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK _vpAreaProc(HWND, UINT, WPARAM, LPARAM);

	void _Init();
	void _InitGL();

public:
	Editor() : _fbxManager(nullptr), _propertyWindow(this), _toolWindow(this), _tools(*this) {}
	~Editor();

	void Run();
	void Frame();
	void Render();
	void RenderViewport(int index, Direction side);

	void Zoom(float);

	void UpdateMousePosition(int vpIndex, unsigned short x, unsigned short y);
	void LeftMouseDown();
	void LeftMouseUp();
	void RightMouseDown();
	void RightMouseUp();

	void KeySubmit();
	void KeyCancel();
	void KeyDelete();

	void ResizeViews(uint16 w, uint16 h);

	String SelectMaterialDialog();
	String SelectModelDialog();

	//For Tools
	inline GameObject& LevelRef() { return _level; }
	inline Viewport& ViewportRef(int id) { return _viewports[id]; }
	inline ObjCamera& CameraRef(int id) { return _viewports[id].CameraRef(); }
	inline PropertyWindow& PropertyWindowRef() { return _propertyWindow; }
};
