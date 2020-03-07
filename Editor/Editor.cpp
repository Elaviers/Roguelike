#include "Editor.hpp"
#include <Engine/Colour.hpp>
#include <Engine/Console.hpp>
#include <Engine/DebugFrustum.hpp>
#include <Engine/DebugLine.hpp>
#include <Engine/DebugManager.hpp>
#include <Engine/DrawUtils.hpp>
#include <Engine/FontManager.hpp>
#include <Engine/InputManager.hpp>
#include <Engine/IO.hpp>
#include <Engine/EntLight.hpp>
#include <Engine/EntRenderable.hpp>
#include <Engine/Mesh_Skeletal.hpp>
#include <Engine/Ray.hpp>
#include <Engine/UISplitter.hpp>
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

	LPCTSTR classNameDummy = TEXT("DUMMY");
	LPCTSTR classNameSide = TEXT("SIDE");
	LPCTSTR classNameConsole = TEXT("CONSOLE");
	LPCTSTR classNameWindow = TEXT("MAINWINDOWCLASS");
	LPCTSTR classNameVPArea = TEXT("VPAREACLASS");

	GLContext dummy;

	{
		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);

		windowClass.lpszClassName = classNameDummy;
		windowClass.lpfnWndProc = ::DefWindowProc;
		windowClass.hInstance = ::GetModuleHandle(NULL);
		::RegisterClassEx(&windowClass);

		windowClass.lpszClassName = classNameVPArea;
		windowClass.lpfnWndProc = _vpAreaProc;
		::RegisterClassEx(&windowClass);

		windowClass.lpszClassName = classNameConsole;
		windowClass.lpfnWndProc = _ConsoleProc;
		windowClass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
		windowClass.hIconSm = ::LoadIcon(NULL, IDI_APPLICATION);
		::RegisterClassEx(&windowClass);

		windowClass.lpszClassName = classNameWindow;
		windowClass.lpfnWndProc = _WindowProc;
		windowClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
		::RegisterClassEx(&windowClass);

		dummy.CreateDummyAndUse(classNameDummy);
		GL::LoadDummyExtensions();

		int screenY = ::GetSystemMetrics(SM_CYSCREEN);
		int cW = 512, cH = 256;

		_consoleWindow.Create(classNameConsole, "Console", this);
		_consoleWindow.SetSizeAndPos(0, screenY - cH - 33, cW, cH);
		_window.Create(classNameWindow, "Window", this);
		_vpArea.Create(classNameVPArea, NULL, this, WS_CHILD | WS_VISIBLE, _window.GetHwnd());
	}

	ResourceSelect::Initialise();

	HierachyWindow::Initialise(_windowBrush);
	_hierachyWindow.Create(&_window);

	//Now we can make a real context
	dummy.Delete();
	_InitGL();

	_fbxManager = FbxManager::Create();
	Engine::Instance().Init(EEngineCreateFlags::ALL, &_level);
	Engine::Instance().pFontManager->AddPath(Utilities::GetSystemFontDir());

	InputManager* inputManager = Engine::Instance().pInputManager;

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
	SharedPointer<const Texture> gradient = Engine::Instance().pTextureManager->Get("ui/gradient");
	SharedPointer<const Texture> radialGradient = Engine::Instance().pTextureManager->Get("ui/radialgrad2");
	SharedPointer<const Texture> splitterTex = Engine::Instance().pTextureManager->Get("editor/tools/splitter");

	_consoleCamera.SetProjectionType(EProjectionType::ORTHOGRAPHIC);
	_consoleCamera.SetZBounds(-100, 100);

	_uiCamera.SetProjectionType(EProjectionType::ORTHOGRAPHIC);
	_uiCamera.SetZBounds(-100, 100);

	_sideUI.SetParent(&_ui);
	_vpAreaUI.SetParent(&_ui);

	UIColour vpColour(Colour::Black, Colour(.1f, .1f, .1f));

	_viewports[0].gridPlane = EDirection::UP;
	_viewports[0].camera.SetProjectionType(EProjectionType::PERSPECTIVE);
	_viewports[0].camera.SetRelativePosition(Vector3(-5.f, 5.f, -5.f));
	_viewports[0].camera.SetRelativeRotation(Vector3(-45.f, 45.f, 0.f));
	_viewports[0].camera.SetZBounds(0.001f, 100.f);
	_viewports[0].ui.SetTexture(radialGradient).SetColour(vpColour).SetParent(&_vpAreaUI).SetZ(100.f).SetFocusOnClick(false);

	_viewports[1].gridPlane = EDirection::UP;
	_viewports[1].camera.SetProjectionType(EProjectionType::ORTHOGRAPHIC);
	_viewports[1].camera.SetScale(32.f);
	_viewports[1].camera.SetZBounds(-10000.f, 10000.f);
	_viewports[1].camera.SetRelativeRotation(Vector3(-90.f, 0.f, 0.f));
	_viewports[1].ui.SetTexture(radialGradient).SetColour(vpColour).SetParent(&_vpAreaUI).SetZ(100.f).SetFocusOnClick(false);

	_viewports[2].gridPlane = EDirection::FORWARD;
	_viewports[2].camera.SetProjectionType(EProjectionType::ORTHOGRAPHIC);
	_viewports[2].camera.SetScale(32.f);
	_viewports[2].camera.SetZBounds(-10000.f, 10000.f);
	_viewports[2].camera.SetRelativeRotation(Vector3(0.f, 0.f, 0.f));
	_viewports[2].ui.SetTexture(radialGradient).SetColour(vpColour).SetParent(&_vpAreaUI).SetZ(100.f).SetFocusOnClick(false);

	_viewports[3].gridPlane = EDirection::RIGHT;
	_viewports[3].camera.SetProjectionType(EProjectionType::ORTHOGRAPHIC);
	_viewports[3].camera.SetScale(32.f);
	_viewports[3].camera.SetZBounds(-10000.f, 10000.f);
	_viewports[3].camera.SetRelativeRotation(Vector3(0.f, -90.f, 0.f));
	_viewports[3].ui.SetTexture(radialGradient).SetColour(vpColour).SetParent(&_vpAreaUI).SetZ(100.f).SetFocusOnClick(false);

	//Note: toolbar is added after the viewport background ui because containers pass events to last elements first
	_toolbar.SetButtonMaterial(Engine::Instance().pMaterialManager->Get("uibutton1")).SetButtonBorderSize(2.f)
		.SetButtonColourFalse(Colour::White).SetButtonColourTrue(Colour::Blue).SetButtonColourHover(Colour(.8f, .8f, .7f)).SetButtonColourHold(Colour::Grey)
		.SetParent(&_ui).SetBounds(0.f, UICoord(1.f, -64.f), 1.f, UICoord(0.f, 64.f));
	_toolbar.AddButton("Select", Engine::Instance().pTextureManager->Get("editor/tools/select"), (uint16)ETool::SELECT);
	_toolbar.AddButton("Brush2D", Engine::Instance().pTextureManager->Get("editor/tools/brush2d"), (uint16)ETool::BRUSH2D);
	_toolbar.AddButton("Brush3D", Engine::Instance().pTextureManager->Get("editor/tools/brush3d"), (uint16)ETool::BRUSH3D);
	_toolbar.AddButton("Entity", Engine::Instance().pTextureManager->Get("editor/tools/entity"), (uint16)ETool::ENTITY);
	//_toolbar.AddButton("Connector", Engine::Instance().pTextureManager->Get("editor/tools/connector"), (uint16)ETool::CONNECTOR);
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


	Engine::Instance().pConsole->Cvars().Add<uint32>("dbga", dbgIDA);
	Engine::Instance().pConsole->Cvars().Add<uint32>("dbgb", dbgIDB);
}

void Editor::_InitGL()
{
	_glContext.Create(_window);
	_glContext.Use(_window);

	GL::LoadExtensions(_window.GetHDC());
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

	_consoleFont = Engine::Instance().pFontManager->Get("consolas");

	_window.SetTitle("Editor");

	_hierachyWindow.Show();
	_vpArea.Focus();

	MSG msg;
	_running = true;
	while (_running)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		Frame();
	}

	_glContext.Delete();
}

void Editor::Frame()
{
	const float moveSpeed = 4.f; //units/s
	const float rotSpeed = 180.f; //degs/s

	_timer.Start();

	if (_deltaTime > .1f) //Do not allow very long time deltas, they screw up the physics
		_deltaTime = 0.1f;

	POINT cursorPos;
	if (::GetCursorPos(&cursorPos))
	{
		RECT client;
		if (::GetWindowRect(_vpArea.GetHwnd(), &client))
		{
			uint16 x = cursorPos.x - client.left;
			uint16 y = _uiCamera.GetViewport()[1] - (cursorPos.y - client.top);
			_ui.OnMouseMove(x, y);
			Engine::UseCursor(_ui.GetCursor());

			UpdateMousePosition(x, y);
		}
	}

	EntCamera &perspCam = _viewports[_activeVP].camera;
	perspCam.RelativeMove(
		perspCam.GetRelativeTransform().GetForwardVector() * _deltaTime * _axisMoveY * moveSpeed
		+ perspCam.GetRelativeTransform().GetRightVector() * _deltaTime * _axisMoveX * moveSpeed
		+ perspCam.GetRelativeTransform().GetUpVector() * _deltaTime * _axisMoveZ * moveSpeed);

	perspCam.AddRelativeRotation(Vector3(_deltaTime * _axisLookY * rotSpeed, _deltaTime * _axisLookX * rotSpeed, 0.f));

	Engine::Instance().pDebugManager->Update(_deltaTime);
	Engine::Instance().pDebugManager->AddToWorld(DebugFrustum::FromCamera(_viewports[0].camera));


	Entity* dbgObjA = Engine::Instance().pWorld->FindChildWithUID(dbgIDA);
	Entity* dbgObjB = Engine::Instance().pWorld->FindChildWithUID(dbgIDB);

	if (dbgObjA && dbgObjB)
	{
		/*
		Vector3 a, b;
		float dist = dbgObjA->MinimumDistanceTo(*dbgObjB, a, b);

		Debug::PrintLine(CSTR(dist));
		Engine::Instance().pDebugManager->AddToWorld(DebugLine(a, b, 0.5f, Colour::Pink, 2.f, 0.5f));
		*/

		Vector3 penetration;
		dbgObjA->Overlaps(*dbgObjB, Vector3(), &penetration);
		Debug::PrintLine(CSTR(penetration));
		//Debug::PrintLine(dbgObjA->Overlaps(*dbgObjB) ? CSTR(dbgObjA->GetPenetrationVector(*dbgObjB)) : "no");
	}

	_level.UpdateAll(_deltaTime);

	_ui.Update(_deltaTime);
	Render();

	_deltaTime = _timer.SecondsSinceStart();
}

void Editor::Render()
{
	_glContext.Use(_vpArea);

	glClearColor(0.3f, 0.3f, 0.4f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);

	RenderUI();

	for (int i = 0; i < VIEWPORTCOUNT; ++i)
		RenderViewport(_viewports[i]);
	
	_vpArea.SwapBuffers();

	RenderConsole();
}

void Editor::RenderUI()
{
	_shaderUnlit.Use();
	_uiCamera.Use();
	_ui.Render();
}

void Editor::RenderConsole()
{
	_glContext.Use(_consoleWindow);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);

	_shaderUnlit.Use();
	_consoleCamera.Use();

	Engine::Instance().pConsole->Render(*_consoleFont, _deltaTime);

	_consoleWindow.SwapBuffers();
}

void Editor::RenderViewport(Viewport& vp)
{
	auto& camera = vp.camera;
	bool persp = camera.GetProjectionType() == EProjectionType::PERSPECTIVE;
	float gridLimit = persp ? 100.f : 0.f;

	const AbsoluteBounds& bounds = vp.ui.GetAbsoluteBounds();
	
	{	//LIT PASS
		_shaderLit.Use();
		camera.Use((int)bounds.x, (int)bounds.y);
		_shaderLit.SetVec4(DefaultUniformVars::vec4Colour, Colour::White);

		_shaderLit.SetInt(DefaultUniformVars::intTextureDiffuse, 0);
		_shaderLit.SetInt(DefaultUniformVars::intTextureNormal, 1);
		_shaderLit.SetInt(DefaultUniformVars::intTextureSpecular, 2);
		_shaderLit.SetInt(DefaultUniformVars::intTextureReflection, 3);

		_shaderLit.SetVec2(DefaultUniformVars::vec2UVOffset, Vector2());
		_shaderLit.SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1, 1));

		_level.RenderAll(_viewports[0].camera, ERenderChannels::PRE_RENDER);
		EntLight::FinaliseLightingForFrame();
		_level.RenderAll(_viewports[0].camera, _litRenderChannels);
	}

	//UNLIT PASS
	_shaderUnlit.Use();
	camera.Use((int)bounds.x, (int)bounds.y);
	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Colour::White);

	_level.RenderAll(_viewports[0].camera, ERenderChannels(_unlitRenderChannels | (_drawEditorFeatures ? ERenderChannels::EDITOR : ERenderChannels::NONE)));
	
	if (_drawEditorFeatures)
	{
		if (!persp) glDepthFunc(GL_ALWAYS);

		glLineWidth(lineW);

		Engine::Instance().pTextureManager->White()->Bind(0);
		_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Colour(.75f, .75f, .75f));
		DrawUtils::DrawGrid(*Engine::Instance().pModelManager, camera, vp.gridPlane, 1.f, 1.f, gridLimit);

		if (persp) glDepthFunc(GL_LEQUAL);
		_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Colour(.5f, .5f, 1.f));
		DrawUtils::DrawGrid(*Engine::Instance().pModelManager, camera, vp.gridPlane, 1.f, 10.f, gridLimit);

		Engine::Instance().pDebugManager->RenderWorld();
	}

	Engine::Instance().pTextureManager->White()->Bind(0);
	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Colour::White);
	if (_currentTool) _currentTool->Render(ERenderChannels::ALL);

	glDepthFunc(GL_ALWAYS);
	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Colour::White);
	_level.RenderAll(_viewports[0].camera, ERenderChannels::SPRITE);
	glDepthFunc(GL_LEQUAL);
}

void Editor::ResizeViews(uint16 w, uint16 h)
{
	_uiCamera.SetViewport(w, h);
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
		_viewports[i].camera.SetViewport((uint16)bounds.w, (uint16)bounds.h);
	}
}

String Editor::SelectMaterialDialog()
{
	String string = ResourceSelect::Dialog(*Engine::Instance().pMaterialManager, *Engine::Instance().pModelManager, "Data/Materials/*.txt", _vpArea.GetHwnd(),
		EResourceType::MATERIAL, _glContext, _shaderLit, _shaderUnlit);

	RECT rect;
	::GetClientRect(_vpArea.GetHwnd(), &rect);
	ResizeViews((uint16)rect.right, (uint16)rect.bottom);

	return string;
}

String Editor::SelectModelDialog()
{
	String string = ResourceSelect::Dialog(*Engine::Instance().pMaterialManager, *Engine::Instance().pModelManager, "Data/Models/*.txt", _vpArea.GetHwnd(),
		EResourceType::MODEL, _glContext, _shaderLit, _shaderUnlit);

	RECT rect;
	::GetClientRect(_vpArea.GetHwnd(), &rect);
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

	if (camera.GetProjectionType() == EProjectionType::PERSPECTIVE)
		_mouseData.rightElement = _mouseData.upElement = _mouseData.forwardElement = 0;
	else
	{
		_mouseData.rightElement = right[0] ? 0 : right[1] ? 1 : 2;
		_mouseData.upElement = up[0] ? 0 : up[1] ? 1 : 2;
		_mouseData.forwardElement = (_mouseData.rightElement != 0 && _mouseData.upElement != 0) ? 0 : (_mouseData.rightElement != 1 && _mouseData.upElement != 1) ? 1 : 2;
	}

	_mouseData.prevUnitX = _mouseData.unitX;
	_mouseData.prevUnitY = _mouseData.unitY;
	int prevViewport = _mouseData.viewport;

	_mouseData.viewport = vpIndex;
	_mouseData.x = x - (uint16)(bounds.x + (bounds.w / 2.f));
	_mouseData.y = y - (uint16)(bounds.y + (bounds.h / 2.f));
	_mouseData.unitX = camera.GetRelativePosition()[_mouseData.rightElement] + (float)_mouseData.x / camera.GetScale();
	_mouseData.unitY = camera.GetRelativePosition()[_mouseData.upElement] + (float)_mouseData.y / camera.GetScale();

	if (_mouseData.isRightDown && _mouseData.viewport == prevViewport)
	{
		if (camera.GetProjectionType() == EProjectionType::ORTHOGRAPHIC)
		{
			Vector3 v;
			v[_mouseData.rightElement] = _mouseData.prevUnitX - _mouseData.unitX;
			v[_mouseData.upElement] = _mouseData.prevUnitY - _mouseData.unitY;
			camera.RelativeMove(v);

			_mouseData.unitX = camera.GetRelativePosition()[_mouseData.rightElement] + (float)_mouseData.x / camera.GetScale();
			_mouseData.unitY = camera.GetRelativePosition()[_mouseData.upElement] + (float)_mouseData.y / camera.GetScale();
		}
		else
		{
			camera.AddRelativeRotation(Vector3((_mouseData.prevUnitY - _mouseData.unitY) / -2.f, (_mouseData.prevUnitX - _mouseData.unitX) / -2.f, 0.f));
		}
	}


	_mouseData.unitX_rounded = _mouseData.unitX < 0.f ? (int)(_mouseData.unitX - 1.f) : (int)_mouseData.unitX;
	_mouseData.unitY_rounded = _mouseData.unitY < 0.f ? (int)(_mouseData.unitY - 1.f) : (int)_mouseData.unitY;

	if (camera.GetProjectionType() == EProjectionType::ORTHOGRAPHIC)
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

	if (_mouseData.viewport >= 0 && _viewports[_mouseData.viewport].camera.GetProjectionType() == EProjectionType::PERSPECTIVE)
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

		if (camera.GetProjectionType() == EProjectionType::ORTHOGRAPHIC)
		{
			float mouseOffsetUnitsX = (float)_mouseData.x / camera.GetScale();
			float mouseOffsetUnitsY = (float)_mouseData.y / camera.GetScale();
			float moveX = mouseOffsetUnitsX - ((float)mouseOffsetUnitsX / (float)amount);
			float moveY = mouseOffsetUnitsY - ((float)mouseOffsetUnitsY / (float)amount);

			camera.SetScale(camera.GetScale() * amount);
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

LRESULT CALLBACK Editor::_ConsoleProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Editor* editor = (Editor*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT create = (LPCREATESTRUCT)lparam;
		editor = (Editor*)create->lpCreateParams;
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)editor);
	}
		break;

	case WM_CLOSE:
		editor->_consoleIsActive = false;
		::ShowWindow(hwnd, SW_HIDE);
		break;

	case WM_SIZE:
	{
		uint16 w = LOWORD(lparam);
		uint16 h = HIWORD(lparam);

		editor->_consoleCamera.SetViewport(w, h);
		editor->_consoleCamera.SetRelativePosition(Vector3(w / 2.f, h / 2.f, 0.f));
	}
		break;

	case WM_CHAR:
		Engine::Instance().pConsole->InputChar((char)wparam);

		break;

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
				editor->_level.DeleteChildren();
				LevelIO::Read(editor->_level, filename.GetData());
			}
			break;

			case ID_FILE_SAVEAS:
			{
				String filename = IO::SaveFileDialog(L"\\Data\\Levels", levelDialogFilter);
				LevelIO::Write(editor->_level, filename.GetData());
			}
			break;

			case ID_IMPORT_ANIMATION:
			{
				Buffer<String> skeletalMeshNames = Engine::Instance().pModelManager->GetAllPossibleKeys();

				for (size_t i = 0; i < skeletalMeshNames.GetSize();)
				{
					SharedPointer<const Model> model = Engine::Instance().pModelManager->Get(skeletalMeshNames[i]);

					if (dynamic_cast<const Mesh_Skeletal*>(model->GetMesh()))
						++i;
					else
						skeletalMeshNames.RemoveIndex(i);
				}

				String choice = StringSelect::Dialog(editor->_window.GetHwnd(), skeletalMeshNames, "Pick a skeleton...");

				if (choice.GetLength())
				{
					SharedPointer<Model> model = Engine::Instance().pModelManager->Get(choice);

					if (model)
					{
						const Mesh_Skeletal* skeletalMesh = dynamic_cast<const Mesh_Skeletal*>(model->GetMesh());

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
				editor->_level.DeleteChildren();
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
			Engine::Instance().pInputManager->Reset();

		break;

	case WM_LBUTTONUP:
		if (!editor->_ui.OnMouseUp())
			editor->LeftMouseUp();
		else
			Engine::Instance().pInputManager->Reset();
		
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

			Engine::Instance().pInputManager->KeyDown((EKeycode)wparam);
		}

		break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		if (!editor->_ui.OnKeyUp((EKeycode)wparam))
			Engine::Instance().pInputManager->KeyUp((EKeycode)wparam);

		break;

	case WM_KILLFOCUS:
		Engine::Instance().pInputManager->Reset();
		break;

	default: return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

#pragma endregion
