#pragma once
#include <Engine/Font.hpp>
#include <Engine/GLContext.hpp>
#include <Engine/GLProgram.hpp>
#include <Engine/LevelIO.hpp>
#include <Engine/Timer.hpp>
#include <Engine/Window.hpp>
#include <Engine/UIToolbar.hpp>
#include <Engine/UISplitter.hpp>
#include "FbxSdk.hpp"
#include "HierachyWindow.hpp"
#include "MouseData.hpp"
#include "ToolBrush2D.hpp"
#include "ToolBrush3D.hpp"
#include "ToolConnector.hpp"
#include "ToolEntity.hpp"
#include "ToolSelect.hpp"
#include "Viewport.hpp"
#include <CommCtrl.h>

#define VIEWPORTCOUNT 4

enum class ETool
{
	SELECT = 0,
	BRUSH2D = 1,
	BRUSH3D = 2,
	ENTITY = 3,
	CONNECTOR = 4
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
	EntCamera _uiCamera;
	SharedPointer<const Font> _consoleFont;
	bool _consoleIsActive = false;

	//Window stuff
	Window _window;
	Window _vpArea;
	HierachyWindow _hierachyWindow;

	UIContainer _ui;
	UIContainer _vpAreaUI;
	UIContainer _sideUI;
	UIContainer _propertyContainer;
	UIContainer _toolPropertyContainer;

	UIToolbar _toolbar;

	Viewport _viewports[VIEWPORTCOUNT];

	//OpenGL
	GLContext _glContext;
	GLProgram _shaderLit;
	GLProgram _shaderUnlit;

	//Editor
	bool _drawEditorFeatures = true;

	ERenderChannels _litRenderChannels = ERenderChannels::NONE;
	ERenderChannels _unlitRenderChannels = ERenderChannels(ERenderChannels::SURFACE | ERenderChannels::UNLIT);

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

	void _OnSplitterDragged(UISplitter&) { _RefreshVPs(); }
	void _RefreshVPs();

	void _OnToolbarItemSelection(UIToolbarItem& item) { SetTool((ETool)item.GetUserData(), false); }

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

	Editor() : _fbxManager(nullptr), _hierachyWindow(this), tools(*this), _level(Entity::EFlags::NONE, "Level") {}
	~Editor();

	void Run();
	void Frame();
	void Render();
	void RenderUI();
	void RenderConsole();
	void RenderViewport(Viewport& vp);

	void Zoom(float);

	//0, 0 = bottom-left of vpArea
	void UpdateMousePosition(unsigned short x, unsigned short y);
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

	void* GetPropertyObject() const;
	void* GetToolPropertyObject() const;

	void ChangePropertyEntity(Entity* ent);

	void RefreshProperties();

	void ClearProperties() { _propertyContainer.Clear(); }
	void ClearToolProperties() { _toolPropertyContainer.Clear(); }

	void SetTool(ETool tool, bool changeToolbarSelection = true);

	//For Tools
	Entity& LevelRef() { return _level; }

	Viewport& GetVP(int index) { return _viewports[index]; }

	void FocusVPArea() { _vpArea.Focus(); }

	static constexpr const float PROPERTY_HEIGHT = 32;
};
