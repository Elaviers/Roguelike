#pragma once
#include <Engine/EngineInstance.hpp>
#include <Engine/World.hpp>
#include <ELGraphics/Font.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELSys/GLContext.hpp>
#include <ELSys/GLProgram.hpp>
#include <ELSys/Timer.hpp>
#include <ELSys/Window.hpp>
#include <ELSys/Window_Win32.hpp>
#include <ELUI/Toolbar.hpp>
#include <ELUI/Splitter.hpp>
#include "FbxSdk.hpp"
#include "HierachyWindow.hpp"
#include "MouseData.hpp"
#include "ToolIso.hpp"
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
	ISO = 1,
	BRUSH2D = 2,
	BRUSH3D = 3,
	ENTITY = 4,
	CONNECTOR = 5
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
	Window_Win32 _window;
	Window_Win32 _vpArea;
	HierachyWindow _hierachyWindow;

	UIContainer _ui;
	UIContainer _vpAreaUI;
	UIContainer _sideUI;
	UIContainer _propertyContainer;
	UIContainer _toolPropertyContainer;

	UIToolbar _toolbar;

	Viewport _viewports[VIEWPORTCOUNT];

	ECursor _prevCursor = ECursor::NONE;

	//Rendering
	GLContext _glContext;
	GLProgram _shaderLit;
	GLProgram _shaderUnlit;
	GLProgram _shaderIDMap;

	RenderQueue _uiQueue;
	RenderQueue _consoleQueue;

	GLCubemap _reflect;

	//Editor
	bool _drawEditorFeatures = true;

	ERenderChannels _litRenderChannels = ERenderChannels::NONE;
	ERenderChannels _unlitRenderChannels = ERenderChannels(ERenderChannels::SURFACE | ERenderChannels::UNLIT);

	World _level;

	Tool *_currentTool = nullptr;

	Viewport* _activeVP = nullptr;

	float _gridZ;

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

	void _OnSplitterDragged(UISplitter&) { _RefreshVPs(); }
	void _RefreshVPs();

	void _OnToolbarItemSelection(UIToolbarItem& item) { SetTool((ETool)item.GetUserData(), false); }

public:
	EngineInstance engine;

	struct _EditorTools
	{
		ToolSelect select;
		ToolIso iso;
		ToolBrush2D brush2D;
		ToolBrush3D brush3D;
		ToolEntity entity;
		ToolConnector connector;

		_EditorTools(Editor& editor) : select(editor), iso(editor), brush2D(editor), brush3D(editor), entity(editor), connector(editor) {}
	} tools;

	Editor() : _fbxManager(nullptr), _hierachyWindow(this), tools(*this), _gridZ(0.f) {}
	~Editor();

	void SetCursor(ECursor cursor);

	void Run();
	void Frame();
	void Render();
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
	World& WorldRef() { return _level; }

	Viewport& GetVP(int index) { return _viewports[index]; }

	void SetGridZ(float gridZ) { _gridZ = gridZ; }

	void FocusVPArea() { _vpArea.Focus(); }

	static constexpr const float PROPERTY_HEIGHT = 32;
};
