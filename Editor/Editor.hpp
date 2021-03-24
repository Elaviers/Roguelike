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
#include "MouseData.hpp"
#include "ToolIso.hpp"
#include "ToolBrush2D.hpp"
#include "ToolBrush3D.hpp"
#include "ToolConnector.hpp"
#include "ToolObject.hpp"
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
	OBJECT = 4,
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
	Projection _consoleCamera;
	Projection _uiCamera;
	SharedPointer<const Font> _consoleFont;
	bool _consoleIsActive = false;

	//Window stuff
	Window_Win32 _window;

	UIContainer _ui;
	UIContainer _vpAreaUI;
	UIContainer _sideUI;
	UIContainer _propertyContainer;
	UIContainer _toolPropertyContainer;

	UIToolbar _toolbar;

	Viewport _viewports[VIEWPORTCOUNT];

	bool _canChangeCursor = false;
	ECursor _prevCursor = ECursor::NONE;

	Vector2T<uint16> _pendingResize;

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

	World _world;

	Tool *_currentTool = nullptr;

	Viewport* _activeVP = nullptr;

	float _gridZ;

	MouseData _mouseData;

	float _axisMoveX = 0.f;
	float _axisMoveY = 0.f;
	float _axisMoveZ = 0.f;
	float _axisLookX = 0.f;
	float _axisLookY = 0.f;

	float _gridUnit = 1.f;

	uint16 _leftPanelSize = 256;

	String _filename;

	//imgui
	bool showUtilsPanel = false;
	bool showDemoPanel = false;
	bool cullingDebug = false;

	static LRESULT CALLBACK _WindowProc(HWND, UINT, WPARAM, LPARAM);

	void _Init();
	void _InitGL();

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
		ToolObject object;
		ToolConnector connector;

		_EditorTools(Editor& editor) : select(editor), iso(editor), brush2D(editor), brush3D(editor), object(editor), connector(editor) {}
	} tools;

	Editor() : _fbxManager(nullptr), tools(*this), _gridZ(0.f), _world(&engine.context) {}
	~Editor();

	void TrySetCursor(ECursor cursor);

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

	void IncreaseGridUnit();
	void DecreaseGridUnit();

	void ToggleConsole();

	String SelectMaterialDialog();
	String SelectModelDialog();

	void* GetPropertyObject() const;
	void* GetToolPropertyObject() const;

	void ChangePropertyWorldObject(WorldObject* ent);

	void RefreshProperties();

	void ClearProperties() { _propertyContainer.Clear(); }
	void ClearToolProperties() { _toolPropertyContainer.Clear(); }

	void SetTool(ETool tool, bool changeToolbarSelection = true);

	//For Tools
	World& WorldRef() { return _world; }

	Viewport& GetVP(int index) { return _viewports[index]; }

	void SetGridZ(float gridZ) { _gridZ = gridZ; }

	static constexpr const float PROPERTY_HEIGHT = 32;
};
