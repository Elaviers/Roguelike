#include "Editor.hpp"
#include <Engine/Console.hpp>
#include <Engine/EntLight.hpp>
#include <Engine/EntRenderable.hpp>
#include <ELGraphics/Colour.hpp>
#include <ELGraphics/DebugFrustum.hpp>
#include <ELGraphics/DebugLine.hpp>
#include <ELGraphics/DebugManager.hpp>
#include <ELGraphics/FontManager.hpp>
#include <ELGraphics/IO.hpp>
#include <ELGraphics/Mesh_Skeletal.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELMaths/Frustum.hpp>
#include <ELMaths/Ray.hpp>
#include <ELSys/InputManager.hpp>
#include <ELSys/System.hpp>
#include <ELSys/Utilities.hpp>
#include <ELUI/Splitter.hpp>
#include "EditorIO.hpp"
#include "resource.h"
#include "ResourceSelect.hpp"
#include "StringSelect.hpp"
#include "UIPropertyManipulator.hpp"

constexpr int tbImageSize = 32;

constexpr GLfloat lineW = 1;

const Buffer<Pair<const wchar_t*>> levelDialogFilter({ Pair<const wchar_t*>(L"Level File", L"*.lvl"), Pair<const wchar_t*>(L"All Files", L"*.*") });

const Buffer<Pair<const wchar_t*>> openAnimationFilter({Pair<const wchar_t*>(L"FBX Scene", L"*.fbx")});
const Buffer<Pair<const wchar_t*>> openModelFilter({Pair<const wchar_t*>(L"FBX Scene", L"*.fbx"), Pair<const wchar_t*>(L"OBJ Model", L"*.obj")});
const Buffer<Pair<const wchar_t*>> openTextureFilter({Pair<const wchar_t*>(L"PNG Image", L"*.png")});

const Buffer<Pair<const wchar_t*>> saveAnimationFilter({ Pair<const wchar_t*>(L"Animation", L"*.anim") });
const Buffer<Pair<const wchar_t*>> saveModelFilter({Pair<const wchar_t*>(L"Mesh", L"*.mesh")});

uint32 dbgIDA, dbgIDB;

Editor::~Editor()
{
	if (_fbxManager)
		_fbxManager->Destroy();
}

#pragma region Init

void Editor::_Init()
{
	HBRUSH _windowBrush = ::CreateSolidBrush(RGB(32, 32, 32));

	LPCTSTR classNameWindow = TEXT("MAINWINDOWCLASS");
	LPCTSTR classNameVPArea = TEXT("VPAREACLASS");

	GLContext dummy;

	{
		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.hInstance = ::GetModuleHandle(NULL);
		windowClass.lpszClassName = classNameVPArea;
		windowClass.lpfnWndProc = _vpAreaProc;
		::RegisterClassEx(&windowClass);

		windowClass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
		windowClass.hIconSm = ::LoadIcon(NULL, IDI_APPLICATION);
		windowClass.lpszClassName = classNameWindow;
		windowClass.lpfnWndProc = _WindowProc;
		windowClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
		::RegisterClassEx(&windowClass);

		dummy.CreateDummyAndUse();
		GL::LoadDummyExtensions();

		int screenY = ::GetSystemMetrics(SM_CYSCREEN);
		int cW = 512, cH = 256;

		_consoleWindow.Create("Console");
		_consoleWindow.SetSizeAndPos(0, screenY - cH - 33, cW, cH);
		_window.Create(classNameWindow, "Window", this);
		_vpArea.Create(classNameVPArea, NULL, this, WS_CHILD | WS_VISIBLE, _window.GetHWND());
	}

	ResourceSelect::Initialise();

	HierachyWindow::Initialise(_windowBrush);
	_hierachyWindow.Create(&_window);

	//Now we can make a real context
	dummy.Delete();
	_InitGL();

	_fbxManager = FbxManager::Create();
	engine.Init(EEngineCreateFlags::ALL, &_level);
	engine.pFontManager->AddPath(Utilities::GetSystemFontDir());

	InputManager* inputManager = engine.pInputManager;

	inputManager->BindKeyAxis(EKeycode::W, &_axisMoveY, 1.f);
	inputManager->BindKeyAxis(EKeycode::S, &_axisMoveY, -1.f);
	inputManager->BindKeyAxis(EKeycode::D, &_axisMoveX, 1.f);
	inputManager->BindKeyAxis(EKeycode::A, &_axisMoveX, -1.f);
	inputManager->BindKeyAxis(EKeycode::SPACE, &_axisMoveZ, 1.f);
	inputManager->BindKeyAxis(EKeycode::SHIFT, &_axisMoveZ, -1.f);

	inputManager->BindKeyAxis(EKeycode::UP, &_axisLookY, 1.f);
	inputManager->BindKeyAxis(EKeycode::DOWN, &_axisLookY, -1.f);
	inputManager->BindKeyAxis(EKeycode::RIGHT, &_axisLookX, 1.f);
	inputManager->BindKeyAxis(EKeycode::LEFT, &_axisLookX, -1.f);

	inputManager->BindKey(EKeycode::ENTER, Callback(this, &Editor::KeySubmit));
	inputManager->BindKey(EKeycode::ESCAPE, Callback(this, &Editor::KeyCancel));
	inputManager->BindKey(EKeycode::DEL, Callback(this, &Editor::KeyDelete));

	inputManager->BindKey(EKeycode::TILDE, Callback(this, &Editor::ToggleConsole));

	_level.onNameChanged +=		Callback(this, &Editor::RefreshLevel);
	_level.onChildChanged +=	Callback(this, &Editor::RefreshLevel);

	//UI
	SharedPointer<const Texture> gradient = engine.pTextureManager->Get("ui/gradient", engine.context);
	SharedPointer<const Texture> radialGradient = engine.pTextureManager->Get("ui/radialgrad2", engine.context);
	SharedPointer<const Texture> splitterTex = engine.pTextureManager->Get("editor/tools/splitter", engine.context);

	_consoleCamera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
	_consoleCamera.GetProjection().SetNearFar(-100, 100);

	_uiCamera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
	_uiCamera.GetProjection().SetNearFar(-100, 100);

	_sideUI.SetParent(&_ui);
	_vpAreaUI.SetParent(&_ui);

	UIColour vpColour(Colour::Black, Colour(.1f, .1f, .1f));

	_viewports[0].gridPlane = EDirection::UP;
	_viewports[0].camera.GetProjection().SetType(EProjectionType::PERSPECTIVE);
	_viewports[0].camera.SetRelativePosition(Vector3(-5.f, 5.f, -5.f));
	_viewports[0].camera.SetRelativeRotation(Vector3(-45.f, 45.f, 0.f));
	_viewports[0].camera.GetProjection().SetNearFar(0.001f, 100.f);
	_viewports[0].ui.SetTexture(radialGradient).SetColour(vpColour).SetParent(&_vpAreaUI).SetZ(100.f).SetFocusOnClick(false);

	_viewports[1].gridPlane = EDirection::UP;
	_viewports[1].camera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
	_viewports[1].camera.GetProjection().SetOrthographicScale(32.f);
	_viewports[1].camera.GetProjection().SetNearFar(-10000.f, 10000.f);
	_viewports[1].camera.SetRelativeRotation(Vector3(-90.f, 0.f, 0.f));
	_viewports[1].ui.SetTexture(radialGradient).SetColour(vpColour).SetParent(&_vpAreaUI).SetZ(100.f).SetFocusOnClick(false);

	_viewports[2].gridPlane = EDirection::FORWARD;
	_viewports[2].camera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
	_viewports[2].camera.GetProjection().SetOrthographicScale(32.f);
	_viewports[2].camera.GetProjection().SetNearFar(-10000.f, 10000.f);
	_viewports[2].camera.SetRelativeRotation(Vector3(0.f, 0.f, 0.f));
	_viewports[2].ui.SetTexture(radialGradient).SetColour(vpColour).SetParent(&_vpAreaUI).SetZ(100.f).SetFocusOnClick(false);

	_viewports[3].gridPlane = EDirection::RIGHT;
	_viewports[3].camera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
	_viewports[3].camera.GetProjection().SetOrthographicScale(32.f);
	_viewports[3].camera.GetProjection().SetNearFar(-10000.f, 10000.f);
	_viewports[3].camera.SetRelativeRotation(Vector3(0.f, -90.f, 0.f));
	_viewports[3].ui.SetTexture(radialGradient).SetColour(vpColour).SetParent(&_vpAreaUI).SetZ(100.f).SetFocusOnClick(false);

	//Note: toolbar is added after the viewport background ui because containers pass events to last elements first
	_toolbar.SetButtonMaterial(engine.pMaterialManager->Get("uibutton1", engine.context)).SetButtonBorderSize(2.f)
		.SetButtonColourFalse(Colour::White).SetButtonColourTrue(Colour::Blue).SetButtonColourHover(Colour(.8f, .8f, .7f)).SetButtonColourHold(Colour::Grey)
		.SetParent(&_ui).SetBounds(0.f, UICoord(1.f, -64.f), 1.f, UICoord(0.f, 64.f));
	_toolbar.AddButton("Select", engine.pTextureManager->Get("editor/tools/select", engine.context), (uint16)ETool::SELECT);
	_toolbar.AddButton("Brush2D", engine.pTextureManager->Get("editor/tools/brush2d", engine.context), (uint16)ETool::BRUSH2D);
	_toolbar.AddButton("Brush3D", engine.pTextureManager->Get("editor/tools/brush3d", engine.context), (uint16)ETool::BRUSH3D);
	_toolbar.AddButton("Entity", engine.pTextureManager->Get("editor/tools/entity", engine.context), (uint16)ETool::ENTITY);
	_toolbar.AddButton("Connector", engine.pTextureManager->Get("editor/tools/connector", engine.context), (uint16)ETool::CONNECTOR);
	_toolbar.onItemSelected += FunctionPointer<void, UIToolbarItem&>(this, &Editor::_OnToolbarItemSelection);

	UIColour splitterColour(Colour::White, Colour(1.f, 1.f, 1.f, 0.5f));
	UISplitter* splitterHoriz = new UISplitter(&_vpAreaUI);
	UISplitter* splitterVert = new UISplitter(&_vpAreaUI);

	splitterHoriz->ShowSiblingAfter(&_viewports[0].ui).ShowSiblingAfter(&_viewports[1].ui).SetIsHorizontal(true).SetTexture(splitterTex).SetColour(splitterColour).SetBounds(0.f, UICoord(.5f, -2.5f), 1.f, UICoord(0.f, 5.f));
	splitterVert->ShowSiblingAfter(&_viewports[1].ui).ShowSiblingAfter(&_viewports[3].ui).SetIsHorizontal(false).SetTexture(splitterTex).SetColour(splitterColour).SetBounds(UICoord(.5f, -2.5f), 0.f, UICoord(0.f, 5.f), 1.f);

	splitterHoriz->onDragged += FunctionPointer<void, UISplitter&>(this, &Editor::_OnSplitterDragged);
	splitterVert->onDragged += FunctionPointer<void, UISplitter&>(this, &Editor::_OnSplitterDragged);

	//This is a laugh
	UIRect* propertyRect = new UIRect(&_sideUI);
	propertyRect->SetTexture(gradient).SetColour(UIColour(Colour(.11f, .1f, .1f), Colour(.22f, .2f, .2f))).SetBounds(0.f, .5f, 1.f, .5f);
	
	UIRect* toolPropertyRect = new UIRect(&_sideUI);
	toolPropertyRect->SetTexture(gradient).SetColour(UIColour(Colour(.1f, .1f, .11f), Colour(.2f, .2f, .22f))).SetBounds(0.f, 0.f, 1.f, .5f);

	_propertyContainer.SetParent(&_sideUI);
	_toolPropertyContainer.SetParent(&_sideUI);

	UISplitter* sideSplitter = new UISplitter(&_ui);
	sideSplitter->ShowSiblingAfter(&_sideUI).SetIsHorizontal(false).SetUseAbsolute(true).SetMin(-1000.f).SetMax(-100.f).SetTexture(splitterTex).SetColour(splitterColour).SetBounds(UICoord(1.f, -400.f), 0.f, UICoord(0.f, 5.f), 1.f);
	sideSplitter->onDragged += FunctionPointer<void, UISplitter&>(this, &Editor::_OnSplitterDragged);

	UISplitter* propertySplitterHoriz = new UISplitter(&_sideUI);
	propertySplitterHoriz->ShowSiblingAfter(propertyRect).ShowSiblingAfter(&_propertyContainer).SetIsHorizontal(true).SetTexture(splitterTex).SetColour(splitterColour).SetBounds(0.f, UICoord(.5f, -2.5f), 1.f, UICoord(0.f, 5.f));
	propertySplitterHoriz->onDragged += FunctionPointer<void, UISplitter&>(this, &Editor::_OnSplitterDragged);

	//Tool data init
	tools.brush2D.Initialise();
	tools.brush3D.Initialise();
	tools.connector.Initialise();
	tools.entity.Initialise();
	tools.select.Initialise();

	SetTool(ETool::SELECT);


	engine.pConsole->Cvars().Add<uint32>("dbga", dbgIDA);
	engine.pConsole->Cvars().Add<uint32>("dbgb", dbgIDB);
}

void Editor::_InitGL()
{
	_glContext.Create(_window);
	_glContext.Use(_window);

	GL::LoadExtensions(_window);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	wglSwapIntervalEXT(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_MULTISAMPLE);

	glLineWidth(lineW);

	_shaderLit.SetMaxLightCount(8);
	_shaderLit.Load("Data/Shaders/Shader.vert", "Data/Shaders/Phong.frag");
	_shaderUnlit.Load("Data/Shaders/Shader.vert", "Data/Shaders/Unlit.frag");
}

#pragma endregion

void Editor::Run()
{
	_Init();
	_window.Show();

	_consoleFont = engine.pFontManager->Get("consolas", engine.context);

	_window.SetTitle("Editor");

	_hierachyWindow.Show();
	_vpArea.Focus();

	WindowEvent e;
	MSG msg;
	_running = true;
	while (_running)
	{
		_timer.Start();
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		while (_consoleWindow.PollEvent(e))
		{
			switch (e.type)
			{
			case WindowEvent::CLOSED:
				_consoleIsActive = false;
				_consoleWindow.Hide();
				break;

			case WindowEvent::RESIZE:
				_consoleCamera.GetProjection().SetDimensions(Vector2T(e.data.resize.w, e.data.resize.h));
				_consoleCamera.SetRelativePosition(Vector3(e.data.resize.w / 2.f, e.data.resize.h / 2.f, 0.f));
				break;

			case WM_CHAR:
				engine.pConsole->InputChar(e.data.character, engine.context);
				break;
			}
		}

		Frame();
		_deltaTime = _timer.SecondsSinceStart();
	}

	_glContext.Delete();
}

void Editor::Frame()
{
	const float moveSpeed = 4.f; //units/s
	const float rotSpeed = 180.f; //degs/s

	if (_deltaTime > .1f) //Do not allow very long time deltas, they screw up the physics
		_deltaTime = 0.1f;

	POINT cursorPos;
	if (::GetCursorPos(&cursorPos))
	{
		RECT client;
		if (::GetWindowRect(_vpArea.GetHWND(), &client))
		{
			uint16 x = cursorPos.x - client.left;
			uint16 y = _uiCamera.GetProjection().GetDimensions().y - (cursorPos.y - client.top);
			_ui.OnMouseMove(x, y);
			
			System::SetCursor(_ui.GetCursor());

			UpdateMousePosition(x, y);
		}
	}

	EntCamera &perspCam = _viewports[_activeVP].camera;
	perspCam.RelativeMove(
		perspCam.GetRelativeTransform().GetForwardVector() * _deltaTime * _axisMoveY * moveSpeed
		+ perspCam.GetRelativeTransform().GetRightVector() * _deltaTime * _axisMoveX * moveSpeed
		+ perspCam.GetRelativeTransform().GetUpVector() * _deltaTime * _axisMoveZ * moveSpeed);

	perspCam.AddRelativeRotation(Vector3(_deltaTime * _axisLookY * rotSpeed, _deltaTime * _axisLookX * rotSpeed, 0.f));

	engine.pDebugManager->Update(_deltaTime);
	engine.pDebugManager->AddToWorld(DebugFrustum::FromCamera(_viewports[0].camera.GetWorldTransform(), _viewports[0].camera.GetProjection()));


	Entity* dbgObjA = engine.pWorld->FindChildWithUID(dbgIDA);
	Entity* dbgObjB = engine.pWorld->FindChildWithUID(dbgIDB);

	if (dbgObjA && dbgObjB)
	{
		/*
		Vector3 a, b;
		float dist = dbgObjA->MinimumDistanceTo(*dbgObjB, a, b);

		Debug::PrintLine(CSTR(dist));
		engine.pDebugManager->AddToWorld(DebugLine(a, b, 0.5f, Colour::Pink, 2.f, 0.5f));
		*/

		Vector3 penetration;
		dbgObjA->Overlaps(*dbgObjB, Vector3(), &penetration);
		Debug::PrintLine(CSTR(penetration));
		//Debug::PrintLine(dbgObjA->Overlaps(*dbgObjB) ? CSTR(dbgObjA->GetPenetrationVector(*dbgObjB)) : "no");
	}

	_level.UpdateAll(_deltaTime);

	_ui.Update(_deltaTime);
	Render();
}

void Editor::Render()
{
	_uiQueue.ClearDynamicQueue();
	_ui.Render(_uiQueue);

	_glContext.Use(_vpArea);
	glClearColor(0.3f, 0.3f, 0.4f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);

	for (int i = 0; i < VIEWPORTCOUNT; ++i)
		RenderViewport(_viewports[i]);

	_shaderUnlit.Use();
	_uiCamera.Use();
	_uiQueue.Render(ERenderChannels::UNLIT, *engine.pMeshManager, *engine.pTextureManager);	
	_vpArea.SwapBuffers();
	

	//Console
	_consoleQueue.ClearDynamicQueue();
	engine.pConsole->Render(_consoleQueue, *_consoleFont, _deltaTime);

	_glContext.Use(_consoleWindow);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);

	_shaderUnlit.Use();
	_consoleCamera.Use();
	_consoleQueue.Render(ERenderChannels::UNLIT, *engine.pMeshManager, *engine.pTextureManager);
	_consoleWindow.SwapBuffers();
}

void Editor::RenderViewport(Viewport& vp)
{
	auto& camera = vp.camera;
	float gridLimit = camera.GetProjection().GetType() == EProjectionType::PERSPECTIVE ? 100.f : 0.f;

	const AbsoluteBounds& bounds = vp.ui.GetAbsoluteBounds();
	
	//The shader and camera are activated here so that EntCamera::Current() is valid
	_shaderLit.Use();
	camera.Use((int)bounds.x, (int)bounds.y);

	vp.renderQueue.ClearDynamicQueue();
	Frustum frustum;
	camera.GetProjection().ToFrustum(camera.GetWorldTransform(), frustum);
	_level.RenderAll(vp.renderQueue, frustum);

	if (_drawEditorFeatures)
	{
		RenderEntry& e = vp.renderQueue.NewDynamicEntry(ERenderChannels::EDITOR);
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
		e.AddSetLineWidth(lineW);

		e.AddSetColour(Colour(.75f, .75f, .75f));
		e.AddGrid(camera.GetWorldTransform(), camera.GetProjection(), vp.gridPlane, 1.f, gridLimit, 0.f);

		e.AddSetColour(Colour(.5f, .5f, 1.f));
		e.AddGrid(camera.GetWorldTransform(), camera.GetProjection(), vp.gridPlane, 10.f, gridLimit, 0.f);

		engine.pDebugManager->RenderWorld(vp.renderQueue);
	}

	if (_currentTool) _currentTool->Render(vp.renderQueue);

	/////////
	_shaderLit.SetInt(DefaultUniformVars::intTextureDiffuse, 0);
	_shaderLit.SetInt(DefaultUniformVars::intTextureNormal, 1);
	_shaderLit.SetInt(DefaultUniformVars::intTextureSpecular, 2);
	_shaderLit.SetInt(DefaultUniformVars::intTextureReflection, 3);
	vp.renderQueue.Render(_litRenderChannels, *engine.pMeshManager, *engine.pTextureManager);

	_shaderUnlit.Use();
	camera.Use((int)bounds.x, (int)bounds.y);
	vp.renderQueue.Render(_unlitRenderChannels | (_drawEditorFeatures ? ERenderChannels::EDITOR : ERenderChannels::NONE), *engine.pMeshManager, *engine.pTextureManager);
}

void Editor::ResizeViews(uint16 w, uint16 h)
{
	_uiCamera.GetProjection().SetDimensions(Vector2T(w, h));
	_uiCamera.SetRelativePosition(Vector3(w / 2.f, h / 2.f, 0.f));
	_ui.SetBounds(0.f, 0.f, UICoord(0.f, w), UICoord(0.f, h));

	if (_running)
		Render();

	_RefreshVPs();
}

void Editor::_RefreshVPs()
{
	for (int i = 0; i < VIEWPORTCOUNT; ++i)
	{
		const AbsoluteBounds& bounds = _viewports[i].ui.GetAbsoluteBounds();
		_viewports[i].camera.GetProjection().SetDimensions(Vector2T((uint16)bounds.w, (uint16)bounds.h));
	}
}

String Editor::SelectMaterialDialog()
{
	String string = ResourceSelect::Dialog(engine, "Data/Materials/*.txt", _vpArea.GetHWND(),
		EResourceType::MATERIAL, _glContext, _shaderLit, _shaderUnlit);

	RECT rect;
	::GetClientRect(_vpArea.GetHWND(), &rect);
	ResizeViews((uint16)rect.right, (uint16)rect.bottom);

	return string;
}

String Editor::SelectModelDialog()
{
	String string = ResourceSelect::Dialog(engine, "Data/Models/*.txt", _vpArea.GetHWND(),
		EResourceType::MODEL, _glContext, _shaderLit, _shaderUnlit);

	RECT rect;
	::GetClientRect(_vpArea.GetHWND(), &rect);
	ResizeViews((uint16)rect.right, (uint16)rect.bottom);

	return string;
}

void* Editor::GetPropertyObject() const
{
	for (size_t i = 0; i < _propertyContainer.GetChildren().GetSize(); ++i)
	{
		UIPropertyManipulator* property = dynamic_cast<UIPropertyManipulator*>(_propertyContainer.GetChildren()[i]);
		if (property) return property->GetObject();
	}

	return nullptr;
}

void* Editor::GetToolPropertyObject() const
{
	for (size_t i = 0; i < _toolPropertyContainer.GetChildren().GetSize(); ++i)
	{
		UIPropertyManipulator* property = dynamic_cast<UIPropertyManipulator*>(_toolPropertyContainer.GetChildren()[i]);
		if (property) return property->GetObject();
	}

	return nullptr;
}

void Editor::ChangePropertyEntity(Entity* ent)
{
	ClearProperties();
	UIPropertyManipulator::AddPropertiesToContainer(PROPERTY_HEIGHT, *this, ent->GetProperties(), ent, _propertyContainer);
}

void Editor::RefreshProperties()
{
	UIPropertyManipulator::RefreshPropertiesInContainer(_propertyContainer);
}

void Editor::SetTool(ETool tool, bool changeToolbar)
{
	if (changeToolbar)
		_toolbar.SelectByUserData((uint16)tool);

	ClearProperties();
	ClearToolProperties();

	Tool* newTool = nullptr;

	switch (tool)
	{
	case ETool::SELECT:
		newTool = &tools.select;
		break;
	case ETool::BRUSH2D:
		newTool = &tools.brush2D;
		break;
	case ETool::BRUSH3D:
		newTool = &tools.brush3D;
		break;
	case ETool::ENTITY:
		newTool = &tools.entity;
		break;
	case ETool::CONNECTOR:
		newTool = &tools.connector;
		break;
	}

	if (_currentTool)
		_currentTool->Deactivate();

	_currentTool = newTool;
	_currentTool->Activate(_propertyContainer, _toolPropertyContainer);
}

#pragma region Input

void Editor::UpdateMousePosition(unsigned short x, unsigned short y)
{
	int vpIndex = -1;

	for (int i = 0; i < VIEWPORTCOUNT; ++i)
	{
		const AbsoluteBounds& bounds = _viewports[i].ui.GetAbsoluteBounds();

		if (x >= bounds.x && y >= bounds.y &&
			x <= (bounds.x + bounds.w) &&
			y <= (bounds.y + bounds.h))
		{
			vpIndex = i;
			break;
		}
	}

	if (vpIndex < 0)
	{
		_mouseData.viewport = -1;
		return;
	}

	const AbsoluteBounds& bounds = _viewports[vpIndex].ui.GetAbsoluteBounds();

	if (_mouseData.viewport != vpIndex)
	{
		if (_mouseData.isLeftDown)
			LeftMouseUp();

		if (_mouseData.isRightDown)
			RightMouseUp();
	}

	EntCamera& camera = _viewports[vpIndex].camera;
	Vector3 right = camera.GetRelativeTransform().GetRightVector();
	Vector3 up = camera.GetRelativeTransform().GetUpVector();

	if (camera.GetProjection().GetType() == EProjectionType::PERSPECTIVE)
		_mouseData.rightElement = _mouseData.upElement = _mouseData.forwardElement = 0;
	else
	{
		_mouseData.rightElement = right.x ? 0 : right.y ? 1 : 2;
		_mouseData.upElement = up.x ? 0 : up.y ? 1 : 2;
		_mouseData.forwardElement = (_mouseData.rightElement != 0 && _mouseData.upElement != 0) ? 0 : (_mouseData.rightElement != 1 && _mouseData.upElement != 1) ? 1 : 2;
	}

	_mouseData.prevX = _mouseData.x;
	_mouseData.prevY = _mouseData.y;
	_mouseData.prevUnitX = _mouseData.unitX;
	_mouseData.prevUnitY = _mouseData.unitY;
	int prevViewport = _mouseData.viewport;

	_mouseData.viewport = vpIndex;
	_mouseData.x = x - (uint16)(bounds.x + (bounds.w / 2.f));
	_mouseData.y = y - (uint16)(bounds.y + (bounds.h / 2.f));

	if (camera.GetProjection().GetType() == EProjectionType::ORTHOGRAPHIC)
	{
		_mouseData.unitX = camera.GetRelativePosition()[_mouseData.rightElement] + (float)_mouseData.x / camera.GetProjection().GetOrthographicScale();
		_mouseData.unitY = camera.GetRelativePosition()[_mouseData.upElement] + (float)_mouseData.y / camera.GetProjection().GetOrthographicScale();
	}
	else
		_mouseData.unitX = _mouseData.unitY = 0.f;

	if (_mouseData.isRightDown && _mouseData.viewport == prevViewport)
	{
		if (camera.GetProjection().GetType() == EProjectionType::ORTHOGRAPHIC)
		{
			Vector3 v;
			v[_mouseData.rightElement] = _mouseData.prevUnitX - _mouseData.unitX;
			v[_mouseData.upElement] = _mouseData.prevUnitY - _mouseData.unitY;
			camera.RelativeMove(v);

			_mouseData.unitX = camera.GetRelativePosition()[_mouseData.rightElement] + (float)_mouseData.x / camera.GetProjection().GetOrthographicScale();
			_mouseData.unitY = camera.GetRelativePosition()[_mouseData.upElement] + (float)_mouseData.y / camera.GetProjection().GetOrthographicScale();
		}
		else
		{
			camera.AddRelativeRotation(Vector3((_mouseData.y - _mouseData.prevY) * .5f, (_mouseData.x - _mouseData.prevX) * .5f, 0.f));
		}
	}

	_mouseData.unitX_rounded = _mouseData.unitX < 0.f ? (int)(_mouseData.unitX - 1.f) : (int)_mouseData.unitX;
	_mouseData.unitY_rounded = _mouseData.unitY < 0.f ? (int)(_mouseData.unitY - 1.f) : (int)_mouseData.unitY;

	if (camera.GetProjection().GetType() == EProjectionType::ORTHOGRAPHIC)
		_window.SetTitle(CSTR(_mouseData.viewport, " Mouse X:", _mouseData.x, " (", _mouseData.unitX, " ) Mouse Y:", _mouseData.y, " (", _mouseData.unitY, ')'));
	else
		_window.SetTitle(CSTR(_mouseData.viewport, " Mouse X:", _mouseData.x, " (", _mouseData.unitX, " ) Mouse Y:", _mouseData.y));

	if (_currentTool)
		_currentTool->MouseMove(_mouseData);
}

void Editor::LeftMouseDown()
{
	_ui.FocusElement(nullptr);

	_mouseData.isLeftDown = true;
	_mouseData.heldUnitX = _mouseData.unitX;
	_mouseData.heldUnitY = _mouseData.unitY;
	_mouseData.heldUnitX_rounded = _mouseData.unitX_rounded;
	_mouseData.heldUnitY_rounded = _mouseData.unitY_rounded;

	if (_currentTool)
		_currentTool->MouseDown(_mouseData);
}

void Editor::LeftMouseUp()
{
	_ui.FocusElement(nullptr);

	_mouseData.isLeftDown = false;

	if (_currentTool)
		_currentTool->MouseUp(_mouseData);
}

void Editor::RightMouseDown()
{
	_mouseData.isRightDown = true;

	if (_mouseData.viewport >= 0 && _viewports[_mouseData.viewport].camera.GetProjection().GetType() == EProjectionType::PERSPECTIVE)
		_activeVP = _mouseData.viewport;
}

void Editor::RightMouseUp()
{
	_mouseData.isRightDown = false;
}

void Editor::KeySubmit()
{
	if (_currentTool)
		_currentTool->KeySubmit();
}

void Editor::KeyCancel()
{
	if (_currentTool)
		_currentTool->Cancel();
}

void Editor::KeyDelete()
{
	if (_currentTool)
		_currentTool->KeyDelete();
}

void Editor::ToggleConsole()
{
	_consoleIsActive = !_consoleIsActive;

	if (_consoleIsActive)
		_consoleWindow.Show();
	else
		_consoleWindow.Hide();
}

void Editor::RefreshLevel()
{
	_hierachyWindow.Refresh(_level);
}

void Editor::Zoom(float amount)
{
	if (_mouseData.viewport >= 0)
	{
		EntCamera& camera = _viewports[_mouseData.viewport].camera;

		if (camera.GetProjection().GetType() == EProjectionType::ORTHOGRAPHIC)
		{
			float mouseOffsetUnitsX = (float)_mouseData.x / camera.GetProjection().GetOrthographicScale();
			float mouseOffsetUnitsY = (float)_mouseData.y / camera.GetProjection().GetOrthographicScale();
			float moveX = mouseOffsetUnitsX - ((float)mouseOffsetUnitsX / (float)amount);
			float moveY = mouseOffsetUnitsY - ((float)mouseOffsetUnitsY / (float)amount);

			camera.GetProjection().SetOrthographicScale(camera.GetProjection().GetOrthographicScale() * amount);
			camera.RelativeMove(camera.GetRelativeTransform().GetRightVector() * moveX + camera.GetRelativeTransform().GetUpVector() * moveY);
		}
	}
}

#pragma endregion

#pragma region WindowsProcs

#include <windowsx.h>

LRESULT CALLBACK AboutProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case IDOK:
			::DestroyWindow(hwnd);
			break;

		}

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

LRESULT CALLBACK Editor::_WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Editor *editor = (Editor*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT create = (LPCREATESTRUCT)lparam;
		editor = (Editor*)create->lpCreateParams;
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)editor);
	}
	break;

	case WM_COMMAND:
		switch (HIWORD(wparam))
		{
		case 0:
			switch (LOWORD(wparam))
			{
			case ID_FILE_OPEN:
			{
				String filename = IO::OpenFileDialog(L"\\Data\\Levels", levelDialogFilter);
				editor->KeyCancel();
				editor->_level.DeleteChildren(editor->engine.context);
				LevelIO::Read(editor->_level, filename.GetData(), editor->engine.context);
			}
			break;

			case ID_FILE_SAVEAS:
			{
				String filename = IO::SaveFileDialog(L"\\Data\\Levels", levelDialogFilter);
				LevelIO::Write(editor->_level, filename.GetData(), editor->engine.context);
			}
			break;

			case ID_IMPORT_ANIMATION:
			{
				Buffer<String> skeletalModelNames = editor->engine.pModelManager->GetAllPossibleKeys();

				for (size_t i = 0; i < skeletalModelNames.GetSize();)
				{
					SharedPointer<const Model> model = editor->engine.pModelManager->Get(skeletalModelNames[i], editor->engine.context);

					if (dynamic_cast<const Mesh_Skeletal*>(model->GetMesh().Ptr()))
						++i;
					else
						skeletalModelNames.RemoveIndex(i);
				}

				String choice = StringSelect::Dialog(editor->_window.GetHWND(), skeletalModelNames, "Pick a skeleton...");

				if (choice.GetLength())
				{
					SharedPointer<Model> model = editor->engine.pModelManager->Get(choice, editor->engine.context);

					if (model)
					{
						const Mesh_Skeletal* skeletalMesh = dynamic_cast<const Mesh_Skeletal*>(model->GetMesh().Ptr());

						if (skeletalMesh)
						{
							String filename = IO::OpenFileDialog(L"\\Data\\Animations", openAnimationFilter);
							if (filename.GetLength() == 0)
								break;

							Animation* animation = EditorIO::ReadFBXAnimation(editor->_fbxManager, filename.GetData(), skeletalMesh->skeleton);

							if (animation)
							{
								String dest = IO::SaveFileDialog(L"\\Data\\Animations", saveAnimationFilter);
								if (dest.GetLength())
									IO::WriteFile(dest.GetData(), animation->GetAsData());
								else
									Debug::Error("Okay, I guess you don't want to import an animation after all...");
							}

						}
					}
				}
			}
			break;

			case ID_IMPORT_MODEL:
			{
				String filename = IO::OpenFileDialog(L"\\Data\\Models", openModelFilter);
				if (filename.GetLength() == 0)
					break;

				String ext = Utilities::GetExtension(filename).ToLower();

				Mesh *mesh = nullptr;

				if (Utilities::GetExtension(filename) == ".fbx")
				{
					mesh = EditorIO::ReadFBXMesh(editor->_fbxManager, filename.GetData());
				}
				else if (Utilities::GetExtension(filename) == ".obj")
				{
					mesh = IO::ReadOBJFile(filename.GetData());
				}

				if (mesh && mesh->IsValid())
				{
					String dest = IO::SaveFileDialog(L"\\Data\\Models", saveModelFilter);

					if (dest.GetLength())
					{
						IO::WriteFile(dest.GetData(), mesh->GetAsData());
					
						Mesh_Skeletal* skeletal = dynamic_cast<Mesh_Skeletal*>(mesh);
						if (skeletal && ::MessageBox(NULL, "Do you want to import the animation too?", "Hey", MB_YESNO) == IDYES)
						{
							Animation* anim = EditorIO::ReadFBXAnimation(editor->_fbxManager, filename.GetData(), skeletal->skeleton);

							if (anim)
							{
								String animDest = IO::SaveFileDialog(L"\\Data\\Animations", saveAnimationFilter);

								if (animDest.GetLength())
									IO::WriteFile(animDest.GetData(), anim->GetAsData());
							}
						}
					}
					else
						Debug::Error("Umm.. I guess you don't want that model imported then.");
				}
				else
					Debug::Error("Sorry, but that ain't a supported model");
			}
			break;

			case ID_IMPORT_TEXTURE:
			{
				String filename = IO::OpenFileDialog(L"\\Data\\Textures", openTextureFilter);

			}
				break;

			case ID_FILE_CLEAR:
				editor->KeyCancel();
				editor->_level.DeleteChildren(editor->engine.context);
				break;

			case ID_VIEW_MATERIALS:
			{
				String unused = editor->SelectMaterialDialog();
			}
			break;

			case ID_VIEW_MODELS:
			{
				String unused = editor->SelectModelDialog();
			}
			break;

			case ID_SHADING_UNLIT:
				editor->_litRenderChannels = ERenderChannels::NONE;
				editor->_unlitRenderChannels = ERenderChannels::SURFACE | ERenderChannels::UNLIT;
				break;

			case ID_SHADING_PHONG:
				editor->_litRenderChannels = ERenderChannels::SURFACE;
				editor->_unlitRenderChannels = ERenderChannels::UNLIT;
				break;

			case ID_DRAWEDITORFEATURES:
				editor->_drawEditorFeatures = !editor->_drawEditorFeatures;

				::CheckMenuItem(::GetMenu(hwnd), ID_DRAWEDITORFEATURES, editor->_drawEditorFeatures ? MF_CHECKED : MF_UNCHECKED);
				break;
			}
			break;
		}

		break;

	case WM_CLOSE:
		::DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		editor->_running = false;
		break;

	case WM_SIZE:
	{
		uint16 w = LOWORD(lparam);
		uint16 h = HIWORD(lparam);
		const int leftW = 256;

		editor->_vpArea.SetSizeAndPos(leftW, 0, w - leftW, h);
		editor->_hierachyWindow.SetSizeAndPos(0, 0, leftW, h);
	}
	break;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

LRESULT CALLBACK Editor::_vpAreaProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Editor *editor = (Editor*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT create = (LPCREATESTRUCT)lparam;
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)create->lpCreateParams);
	}
	break;

	case WM_SIZE:
		editor->ResizeViews(LOWORD(lparam), HIWORD(lparam));
		editor->_RefreshVPs();
		break;

	case WM_MOUSEWHEEL:
		if ((signed short)HIWORD(wparam) > 0)
			editor->Zoom(1.25f);
		else
			editor->Zoom(.75f);
		break;

	case WM_LBUTTONDOWN:
		::SetFocus(hwnd);
		if (!editor->_ui.OnMouseDown())
			editor->LeftMouseDown();
		else
			editor->engine.pInputManager->Reset();

		break;

	case WM_LBUTTONUP:
		if (!editor->_ui.OnMouseUp())
			editor->LeftMouseUp();
		else
			editor->engine.pInputManager->Reset();
		
		break;

	case WM_RBUTTONDOWN:
		::SetFocus(hwnd);
		editor->RightMouseDown();
		break;

	case WM_RBUTTONUP:
		editor->RightMouseUp();
		break;

	case WM_CHAR:
		editor->_ui.OnCharInput((char)wparam);
		break;

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		if (!editor->_ui.OnKeyDown((EKeycode)wparam))
		{
			if (lparam & (1 << 30))
				break; //Key repeats ignored

			editor->engine.pInputManager->KeyDown((EKeycode)wparam);
		}

		break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		if (!editor->_ui.OnKeyUp((EKeycode)wparam))
			editor->engine.pInputManager->KeyUp((EKeycode)wparam);

		break;

	case WM_KILLFOCUS:
		editor->engine.pInputManager->Reset();
		break;

	default: return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

#pragma endregion
