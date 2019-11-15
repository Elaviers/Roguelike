#pragma once
#include <Engine/Font.hpp>
#include <Engine/GLContext.hpp>
#include <Engine/GLProgram.hpp>
#include <Engine/LevelIO.hpp>
#include <Engine/Timer.hpp>
#include <Engine/Window.hpp>
#include "FbxSdk.hpp"
#include "HierachyWindow.hpp"
#include "MouseData.hpp"
#include "PropertyWindow.hpp"
#include "ToolBrush2D.hpp"
#include "ToolBrush3D.hpp"
#include "ToolConnector.hpp"
#include "ToolEntity.hpp"
#include "ToolSelect.hpp"
#include "ToolWindow.hpp"
#include "Viewport.hpp"
#include <CommCtrl.h>

#define VIEWPORTCOUNT 4

enum ToolEnum
{
	TOOL_SELECT = 0,
	TOOL_BRUSH2D = 1,
	TOOL_BRUSH3D = 2,
	TOOL_ENTITY = 3,
	TOOL_CONNECTOR = 4
};

class Editor
{
private:
	bool _running = false;
	float _deltaTime = 0.f;

	//Engine
	Timer _timer;

	FbxManager* _fbxManager;

	//Console
	Window _consoleWindow;
	EntCamera _consoleCamera;
	Font* _consoleFont = nullptr;
	bool _consoleIsActive = false;

	//Window stuff
	Window _window;
	Window _vpArea;
	HierachyWindow _hierachyWindow;
	PropertyWindow _propertyWindow;
	ToolWindow _toolWindow;

	HBRUSH _windowBrush = NULL;
	HWND _toolbar = NULL;
	HIMAGELIST _tbImages = NULL;

	EntCamera _uiCam;

	Viewport _viewports[VIEWPORTCOUNT];

	//OpenGL
	GLContext _glContext;
	GLProgram _shaderLit;
	GLProgram _shaderUnlit;

	//Editor
	bool _drawEditorFeatures = true;

	EnumRenderChannel _litRenderChannels = RenderChannel::NONE;
	EnumRenderChannel _unlitRenderChannels = EnumRenderChannel(RenderChannel::SURFACE | RenderChannel::UNLIT);

	Entity _level;

	Tool *_currentTool = nullptr;

	int _activeVP = 0;


	MouseData _mouseData;

	float _axisMoveX = 0.f;
	float _axisMoveY = 0.f;
	float _axisMoveZ = 0.f;
	float _axisLookX = 0.f;
	float _axisLookY = 0.f;

	static LRESULT CALLBACK _ConsoleProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK _WindowProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK _vpAreaProc(HWND, UINT, WPARAM, LPARAM);

	void _Init();
	void _InitGL();

public:
	struct _EditorTools
	{
		ToolSelect select;
		ToolBrush2D brush2D;
		ToolBrush3D brush3D;
		ToolEntity entity;
		ToolConnector connector;

		_EditorTools(Editor& editor) : select(editor), brush2D(editor), brush3D(editor), entity(editor), connector(editor) {}
	} tools;

	Editor() : _fbxManager(nullptr), _hierachyWindow(this), _propertyWindow(this), _toolWindow(this), tools(*this), _level(0, "Level") {}
	~Editor();

	void Run();
	void Frame();
	void Render();
	void RenderConsole();
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

	void ToggleConsole();
	void RefreshLevel();

	void ResizeViews(uint16 w, uint16 h);

	String SelectMaterialDialog();
	String SelectModelDialog();

	void SetTool(ToolEnum tool, bool changeToolbarSelection = true);

	//For Tools
	inline Entity& LevelRef() { return _level; }
	inline Viewport& ViewportRef(int id) { return _viewports[id]; }
	inline EntCamera& CameraRef(int id) { return _viewports[id].CameraRef(); }
	inline PropertyWindow& PropertyWindowRef() { return _propertyWindow; }
};
