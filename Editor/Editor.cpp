#include "Editor.hpp"
#include "EditorUtil.hpp"
#include <Engine/Console.hpp>
#include <Engine/EngineUtilities.hpp>
#include <Engine/OLight.hpp>
#include <Engine/ORenderable.hpp>
#include <Engine/imgui/imgui.h>
#include <Engine/imgui/imgui_internal.h>
#include <Engine/imgui/imgui_impl_win32.h>
#include <Engine/imgui/imgui_impl_opengl3.h>
#include <ELCore/TextProvider.hpp>
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
#include <ELPhys/Collision.hpp>
#include <ELSys/InputManager.hpp>
#include <ELSys/System.hpp>
#include <ELSys/Utilities.hpp>
#include <ELUI/Splitter.hpp>
#include <ELUI/TabBook.hpp>
#include "EditorIO.hpp"
#include "resource.h"
#include "ResourceSelect.hpp"
#include "StringSelect.hpp"
#include "UIPropertyManipulator.hpp"

#define IDMAP_DEBUG 0

constexpr int tbImageSize = 32;

constexpr GLfloat lineW = 1;

const Buffer<Pair<const wchar_t*>> levelDialogFilter({ Pair<const wchar_t*>(L"Level File", L"*.lvl"), Pair<const wchar_t*>(L"All Files", L"*.*") });

const Buffer<Pair<const wchar_t*>> openAnimationFilter({Pair<const wchar_t*>(L"FBX Scene", L"*.fbx"), Pair<const wchar_t*>(L"Animation", L"*.anim")});
const Buffer<Pair<const wchar_t*>> openModelFilter({Pair<const wchar_t*>(L"FBX Scene", L"*.fbx"), Pair<const wchar_t*>(L"OBJ Model", L"*.obj"), Pair<const wchar_t*>(L"Mesh", L"*.mesh") });
const Buffer<Pair<const wchar_t*>> openTextureFilter({Pair<const wchar_t*>(L"PNG Image", L"*.png")});

const Buffer<Pair<const wchar_t*>> saveAnimationFilter({ Pair<const wchar_t*>(L"Animation", L"*.anim") });
const Buffer<Pair<const wchar_t*>> saveModelFilter({Pair<const wchar_t*>(L"Mesh", L"*.mesh")});

uint32 dbgIDA, dbgIDB;

Editor::~Editor()
{
	if (_fbxManager)
		_fbxManager->Destroy();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

#pragma region Init

void Editor::_Init()
{
	HBRUSH _windowBrush = ::CreateSolidBrush(RGB(32, 32, 32));

	LPCTSTR classNameWindow = TEXT("EDITORWINDOW");

	GLContext dummy;

	{
		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.hInstance = ::GetModuleHandle(NULL);
		windowClass.lpszClassName = classNameWindow;
		windowClass.lpfnWndProc = _WindowProc;
		windowClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
		windowClass.hIcon = windowClass.hIconSm = ::LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
		::RegisterClassEx(&windowClass);

		dummy.CreateDummyAndUse();
		GL::LoadDummyExtensions();

		int screenY = ::GetSystemMetrics(SM_CYSCREEN);
		int cW = 512, cH = 256;

		_consoleWindow.Create("Console");
		_consoleWindow.SetSizeAndPos(0, screenY - cH - 33, cW, cH);
		_window.Create(classNameWindow, "Window", WS_OVERLAPPEDWINDOW, NULL, this);
	}

	ResourceSelect::Initialise();

	//Now we can make a real context
	dummy.Delete();
	_InitGL();

	_fbxManager = FbxManager::Create();
	engine.Init(EEngineCreateFlags::ALL);
	engine.pFontManager->AddPath(Utilities::GetSystemFontDir());

	InputManager* inputManager = engine.pInputManager;

	inputManager->BindKeyAxis(EKeycode::W, &_axisMoveY, 1.f);
	inputManager->BindKeyAxis(EKeycode::S, &_axisMoveY, -1.f);
	inputManager->BindKeyAxis(EKeycode::D, &_axisMoveX, 1.f);
	inputManager->BindKeyAxis(EKeycode::A, &_axisMoveX, -1.f);
	inputManager->BindKeyAxis(EKeycode::SPACE, &_axisMoveZ, 1.f);
	inputManager->BindKeyAxis(EKeycode::LSHIFT, &_axisMoveZ, -1.f);

	inputManager->BindKeyAxis(EKeycode::UP, &_axisLookY, 1.f);
	inputManager->BindKeyAxis(EKeycode::DOWN, &_axisLookY, -1.f);
	inputManager->BindKeyAxis(EKeycode::RIGHT, &_axisLookX, 1.f);
	inputManager->BindKeyAxis(EKeycode::LEFT, &_axisLookX, -1.f);

	inputManager->BindKeyDown(EKeycode::ENTER, Callback(&Editor::KeySubmit, *this));
	inputManager->BindKeyDown(EKeycode::ESCAPE, Callback(&Editor::KeyCancel, *this));
	inputManager->BindKeyDown(EKeycode::DEL, Callback(&Editor::KeyDelete, *this));

	inputManager->BindKeyDown(EKeycode::TILDE, Callback(&Editor::ToggleConsole, *this));

	inputManager->BindKeyDown(EKeycode::SQBRACKETLEFT, Callback(&Editor::DecreaseGridUnit, *this));
	inputManager->BindKeyDown(EKeycode::SQBRACKETRIGHT, Callback(&Editor::IncreaseGridUnit, *this));

	inputManager->BindKeyDown(EKeycode::F1, [this]() { showUtilsPanel = !showUtilsPanel; });

	_world.Initialise();

	//UI
	SharedPointer<const Font> vpFont = engine.pFontManager->Get("consolas", engine.context);

	SharedPointer<const Texture> gradient = engine.pTextureManager->Get("ui/gradient", engine.context);
	SharedPointer<const Texture> radialGradient = engine.pTextureManager->Get("ui/radialgrad", engine.context);
	SharedPointer<const Texture> splitterTex = engine.pTextureManager->Get("editor/tools/splitter", engine.context);

	_consoleCamera.SetType(EProjectionType::ORTHOGRAPHIC);
	_consoleCamera.SetNearFar(-100, 100);

	_uiCamera.SetType(EProjectionType::ORTHOGRAPHIC);
	_uiCamera.SetNearFar(0, 1000000);

	UITabBook* tabbook = new UITabBook(&_ui);

	auto tabMat = engine.pMaterialManager->Get("editor/uitab", engine.context);
	tabbook->SetTabMaterial(tabMat).SetTabMaterialHover(tabMat).SetTabMaterialSelected(tabMat);
	tabbook->SetTabColour(UIColour(Colour(0.f, 0.f, 0.5f), Colour(0.f, 0.f, .6f), Colour::White));
	tabbook->SetTabColourHover(UIColour(Colour(0.f, 0.f, 0.7f), Colour(0.f, 0.f, .8f), Colour::White));
	tabbook->SetTabColourSelected(UIColour(Colour(0.f, 0.f, 0.2f), Colour(0.f, 0.f, .3f), Colour(1.f, .8f, 0.f)));
	tabbook->SetTabFont(vpFont);

	UIContainer* mainPage = new UIContainer(tabbook);
	tabbook->SetTabBorderSize(12.f);
	tabbook->SetTabName(*mainPage, engine.pTextProvider->Get("editor_page_level"));
	tabbook->SetActiveTab(mainPage);

	_sideUI.SetParent(mainPage);
	_vpAreaUI.SetParent(mainPage);

	UIColour vpColour(Colour::Black, Colour(.1f, .1f, .1f));
	for (int i = 0; i < VIEWPORTCOUNT; ++i)
	{
		_viewports[i].Initialise(*engine.pTextProvider);
		_viewports[i].ui.SetParent(&_vpAreaUI);
		_viewports[i].bg.SetTexture(radialGradient).SetColour(vpColour).SetZ(_uiCamera.GetFar()).SetFocusOnClick(false);
		_viewports[i].SetFont(vpFont);
	}

	_viewports[0].SetCameraType(Viewport::ECameraType::ISOMETRIC);
	_viewports[1].SetCameraType(Viewport::ECameraType::ORTHO_Y);
	_viewports[2].SetCameraType(Viewport::ECameraType::ORTHO_Z);
	_viewports[3].SetCameraType(Viewport::ECameraType::PERSPECTIVE);

	//Note: toolbar is added after the viewport background ui because containers pass events to last elements first
	_toolbar.SetButtonMaterial(engine.pMaterialManager->Get("uibutton1", engine.context)).SetButtonBorderSize(2.f)
		.SetButtonColourFalse(UIColour(Colour::White, Colour::Black, Colour::Black))
		.SetButtonColourTrue(UIColour(Colour::Blue, Colour::Black, Colour::Black))
		.SetButtonColourHover(UIColour(Colour(.8f, .8f, .7f), Colour::Black, Colour::Black))
		.SetButtonColourHold(UIColour(Colour::Grey, Colour::Black, Colour::Black))
		.SetParent(mainPage).SetBounds(UIBounds(0.f, UICoord(1.f, -64.f), 1.f, UICoord(0.f, 64.f)));
	_toolbar.AddButton(Text("Select"), engine.pTextureManager->Get("editor/tools/select", engine.context), (uint16)ETool::SELECT);
	_toolbar.AddButton(Text("Iso"), engine.pTextureManager->Get("editor/tools/iso", engine.context), (uint16)ETool::ISO);
	_toolbar.AddButton(Text("Brush2D"), engine.pTextureManager->Get("editor/tools/brush2d", engine.context), (uint16)ETool::BRUSH2D);
	_toolbar.AddButton(Text("Brush3D"), engine.pTextureManager->Get("editor/tools/brush3d", engine.context), (uint16)ETool::BRUSH3D);
	_toolbar.AddButton(Text("Object"), engine.pTextureManager->Get("editor/tools/object", engine.context), (uint16)ETool::OBJECT);
	//_toolbar.AddButton("Connector", _engine.pTextureManager->Get("editor/tools/connector", _engine.context), (uint16)ETool::CONNECTOR);
	_toolbar.onItemSelected += Function(&Editor::_OnToolbarItemSelection, *this);

	UIColour splitterColour(Colour::White, Colour(1.f, 1.f, 1.f, 0.5f));
	UISplitter* splitterHoriz = new UISplitter(&_vpAreaUI);
	UISplitter* splitterVert = new UISplitter(&_vpAreaUI);

	splitterHoriz->ShowSiblingAfter(&_viewports[0].ui).ShowSiblingAfter(&_viewports[1].ui).SetIsHorizontal(true).SetTexture(splitterTex).SetColour(splitterColour).SetBounds(UIBounds(0.f, UICoord(.5f, -2.5f), 1.f, UICoord(0.f, 5.f)));
	splitterVert->ShowSiblingAfter(&_viewports[1].ui).ShowSiblingAfter(&_viewports[3].ui).SetIsHorizontal(false).SetTexture(splitterTex).SetColour(splitterColour).SetBounds(UIBounds(UICoord(.5f, -2.5f), 0.f, UICoord(0.f, 5.f), 1.f));

	auto p = [this](UISplitter&) { _RefreshVPs(); };
	Function<void, UISplitter&> splitterDragged = Function<void, UISplitter&>(p);

	splitterHoriz->onDragged += splitterDragged;
	splitterVert->onDragged += splitterDragged;

	//This is a laugh
	UIRect* propertyRect = new UIRect(&_sideUI);
	propertyRect->SetTexture(gradient).SetColour(UIColour(Colour(.11f, .1f, .1f), Colour(.22f, .2f, .2f))).SetBounds(UIBounds(0.f, .5f, 1.f, .5f));

	UIRect* toolPropertyRect = new UIRect(&_sideUI);
	toolPropertyRect->SetTexture(gradient).SetColour(UIColour(Colour(.1f, .1f, .11f), Colour(.2f, .2f, .22f))).SetBounds(UIBounds(0.f, 0.f, 1.f, .5f));

	_propertyContainer.SetParent(&_sideUI);
	_toolPropertyContainer.SetParent(&_sideUI);

	UISplitter* sideSplitter = new UISplitter(mainPage);
	sideSplitter->ShowSiblingAfter(&_sideUI).SetIsHorizontal(false).SetUseAbsolute(true).SetMin(-1000.f).SetMax(-100.f).SetTexture(splitterTex).SetColour(splitterColour).SetBounds(UIBounds(UICoord(1.f, -400.f), 0.f, UICoord(0.f, 5.f), 1.f));
	sideSplitter->onDragged += splitterDragged;

	UISplitter* propertySplitterHoriz = new UISplitter(&_sideUI);
	propertySplitterHoriz->ShowSiblingAfter(propertyRect).ShowSiblingAfter(&_propertyContainer).SetIsHorizontal(true).SetTexture(splitterTex).SetColour(splitterColour).SetBounds(UIBounds(0.f, UICoord(.5f, -2.5f), 1.f, UICoord(0.f, 5.f)));
	propertySplitterHoriz->onDragged += splitterDragged;

	//Tool data init
	tools.select.Initialise();
	tools.iso.Initialise();
	tools.brush2D.Initialise();
	tools.brush3D.Initialise();
	tools.connector.Initialise();
	tools.object.Initialise();

	SetTool(ETool::SELECT);

	engine.pConsole->Cvars().Add<uint32>("dbga", dbgIDA);
	engine.pConsole->Cvars().Add<uint32>("dbgb", dbgIDB);

	//Third party (IMGUI)
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	
	ImGui::StyleColorsDark();

	ImGui::GetIO().IniFilename = NULL;

	ImGui_ImplWin32_Init(_window.GetHWND());
	ImGui_ImplOpenGL3_Init("#version 410");
}

void Editor::_InitGL()
{
	_glContext.Create(_window);
	_glContext.Use(_window);

	GL::LoadExtensions(_window);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	wglSwapIntervalEXT(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glLineWidth(lineW);

	_shaderLit.Load("Data/Shaders/Shader.vert", "Data/Shaders/Phong.frag");
	_shaderUnlit.Load("Data/Shaders/Shader.vert", "Data/Shaders/Unlit.frag");
	_shaderIDMap.Load("Data/Shaders/Basic.vert", "Data/Shaders/IDMap.frag");

	TextureData faces[6];
	for (int i = 0; i < 6; ++i)
	{
		faces[i].data = new byte[4]();
		faces[i].width = 1;
		faces[i].height = 1;
	}

	_reflect.Create(faces);

	for (int i = 0; i < 6; ++i)
		delete faces[i].data;
}

#pragma endregion

void Editor::TrySetCursor(ECursor cursor)
{
	if (_canChangeCursor)
	{
		if (cursor != _prevCursor)
		{
			_prevCursor = cursor;
			System::SetCursor(cursor);
		}
		
		_canChangeCursor = false;
	}
}

void Editor::Run()
{
	_Init();
	_window.Show();

	_consoleFont = engine.pFontManager->Get("arial", engine.context);

	_window.SetTitle("Editor");

	WindowEvent e;
	MSG msg;
	_running = true;
	while (_running)
	{
		_timer.Start();

		while (_window.PollEvent(e))
		{
			switch (e.type)
			{
			case WindowEvent::CLOSED:
				_running = false;
				break;

			case WindowEvent::RESIZE:
				_pendingResize = Vector2T<uint16>(e.data.resize.w, e.data.resize.h);
				break;

			case WindowEvent::FOCUS_LOST:
				engine.pInputManager->Reset();
				break;

			case WindowEvent::SCROLLWHEEL:
				if (e.data.scrollWheel.lines > 0)
					Zoom(1.25f);
				else
					Zoom(.75f);
				break;

			case WindowEvent::LEFTMOUSEDOWN:
				if (!_ui.KeyDown(EKeycode::MOUSE_LEFT))
					LeftMouseDown();
				else
					engine.pInputManager->Reset();

				break;

			case WindowEvent::LEFTMOUSEUP:
				if (!_ui.KeyUp(EKeycode::MOUSE_LEFT))
					LeftMouseUp();
				else
					engine.pInputManager->Reset();

				break;

			case WindowEvent::RIGHTMOUSEDOWN:
				RightMouseDown();
				break;

			case WindowEvent::RIGHTMOUSEUP:
				RightMouseUp();
				break;

			case WindowEvent::CHAR:
				_ui.InputChar((char)e.data.character);
				break;

			case WindowEvent::KEYDOWN:
				if (!_ui.KeyDown(e.data.keyDown.key))
				{
					if (e.data.keyDown.isRepeat)
						break; //Key repeats ignored

					engine.pInputManager->KeyDown(e.data.keyDown.key);
				}

				break;

			case WindowEvent::KEYUP:
				if (!_ui.KeyUp(e.data.keyUp.key))
					engine.pInputManager->KeyUp(e.data.keyUp.key);

				break;
			}
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
				_consoleCamera.SetDimensions(Vector2T(e.data.resize.w, e.data.resize.h));
				break;

			case WindowEvent::CHAR:
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

	if (_pendingResize.x && _pendingResize.y)
	{
		_uiCamera.SetDimensions(_pendingResize);
		_ui.SetBounds(UIBounds(UICoord(0, _leftPanelSize), 0.f, UICoord(0.f, _pendingResize.x - _leftPanelSize), UICoord(0.f, _pendingResize.y)));

		_RefreshVPs();
		_pendingResize = Vector2T<uint16>();
	}

	{
		//IMGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//Sidebar
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2(_leftPanelSize, _ui.GetAbsoluteBounds().h));
			
			ImGui::Begin("SIDEBAR", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
			
			Buffer<WorldObject*> selection;

			if (_currentTool == &tools.select)
				tools.select.GetSelection(selection);

			if (EngineUtilities::WorldIMGUI(_world, selection))
			{
				OGeometryCollection* g;
				if ((selection.GetSize() == 1) && (g = dynamic_cast<OGeometryCollection*>(selection[0])))
				{
					tools.iso.SetTarget(g);

					if (_currentTool != &tools.iso)
						SetTool(ETool::ISO);
				}
				else
				{
					if (_currentTool != &tools.select)
						SetTool(ETool::SELECT);

					tools.select.SetSelection(selection);
				}
			}
			
			ImGui::End();
		}

		if (showDemoPanel)
			ImGui::ShowDemoWindow(&showDemoPanel);

		if (showUtilsPanel)
		{
			ImGui::Begin("Editor Utilities", &showUtilsPanel);
			ImGui::Text("Average frametime is %.3f ms (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Checkbox("Cullinng debug", &cullingDebug);
			if (ImGui::Button("Test ImGui"))
				showDemoPanel = true;;

			ImGui::End();
		}
	}

	if (!ImGui::GetIO().WantCaptureMouse)
	{
		POINT clientCursorPos;
		if (::GetCursorPos(&clientCursorPos) && ::ScreenToClient(_window.GetHWND(), &clientCursorPos))
		{
			RECT client;
			if (::GetClientRect(_window.GetHWND(), &client))
			{
				if (clientCursorPos.x >= client.left && clientCursorPos.y >= client.top && clientCursorPos.x < client.right && clientCursorPos.y < client.bottom)
				{
					uint16 x = (uint16)(clientCursorPos.x - client.left);
					uint16 y = _uiCamera.GetDimensions().y - (uint16)(clientCursorPos.y - client.top);
					_ui.OnMouseMove(false, x, y);

					_prevCursor = System::GetCursor();
					_canChangeCursor = ::GetFocus() == _window.GetHWND();
					UpdateMousePosition(x, y);
					TrySetCursor(_ui.GetCursor());
					_canChangeCursor = false;
				}
			}
		}
	}

	if (_activeVP)
	{
		_activeVP->cameraTransform.Move(
			_activeVP->cameraTransform.GetForwardVector() * _deltaTime * _axisMoveY * moveSpeed
			+ _activeVP->cameraTransform.GetRightVector() * _deltaTime * _axisMoveX * moveSpeed
			+ _activeVP->cameraTransform.GetUpVector() * _deltaTime * _axisMoveZ * moveSpeed);

		_activeVP->cameraTransform.AddRotation(Vector3(_deltaTime * _axisLookY * rotSpeed, _deltaTime * _axisLookX * rotSpeed, 0.f));
	}

	engine.pDebugManager->Update(_deltaTime);

	if (cullingDebug)
		engine.pDebugManager->AddToWorld(DebugFrustum::FromFrustum(_viewports[0].cameraFrustum));

	WorldObject* dbgObjA = _world.FindObject(dbgIDA);
	WorldObject* dbgObjB = _world.FindObject(dbgIDB);

	if (dbgObjA && dbgObjB)
	{
		Vector3 a, b;
		float dist = _world.CalculateClosestPoints(*dbgObjA, *dbgObjB, a, b);
		
		Debug::PrintLine(CSTR(dist));
		engine.pDebugManager->AddToWorld(DebugLine(a, b, 0.5f, Colour::Pink, 2.f, 0.5f));

		Vector3 penetration;
		EOverlapResult result = _world.ObjectsOverlap(*dbgObjA, *dbgObjB, &penetration);
		
		if (result == EOverlapResult::OVERLAPPING)
			Debug::PrintLine(CSTR(penetration));
		else if (result == EOverlapResult::TOUCHING)
			Debug::PrintLine("Touch");

		//Debug::PrintLine(dbgObjA->Overlaps(*dbgObjB) ? CSTR(dbgObjA->GetPenetrationVector(*dbgObjB)) : "no");
	}

	_world.Update(_deltaTime);
	_ui.Update(_deltaTime);
	if (_currentTool) _currentTool->Update(_deltaTime);

	Render();

	//calculate hover WorldObject/geometry
	if (_mouseData.viewport)
	{
		uint32 x = _mouseData.x + (uint32)_mouseData.viewport->ui.GetAbsoluteBounds().w / 2;
		uint32 y = _mouseData.y + (uint32)_mouseData.viewport->ui.GetAbsoluteBounds().h / 2;

		uint32 colour[4];
		_mouseData.viewport->SampleFramebuffer(x, y, colour);
		_mouseData.hoverObject = _world.DecodeIDMapValue(colour[0], colour[1]);
		_mouseData.hoverData[0] = colour[0];
		_mouseData.hoverData[1] = colour[1];
		_mouseData.hoverData[2] = colour[2];
		_mouseData.hoverData[3] = colour[3];
	}
}

void Editor::Render()
{
	_uiQueue.Clear();
	_uiQueue.CreateCameraEntry(_uiCamera, Transform(Vector3(_uiCamera.GetDimensions().x / 2.f, _uiCamera.GetDimensions().y / 2.f, 0.f)));
	_ui.Render(_uiQueue);

	_glContext.Use(_window);
	glClearColor(0.3f, 0.3f, 0.4f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);

	for (int i = 0; i < VIEWPORTCOUNT; ++i)
		RenderViewport(_viewports[i]);

	_shaderUnlit.Use();
	_uiQueue.Render(ERenderChannels::UNLIT, engine.pMeshManager, engine.pTextureManager, 0);

	//IMGUI RENDER
	if (ImGui::GetCurrentContext() && ImGui::GetCurrentContext()->WithinFrameScope)
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	_window.SwapBuffers();
	

	//ID MAP
	if (_mouseData.viewport)
	{
		Viewport& vp = *_mouseData.viewport;

		vp.renderQueue.Clear();
		vp.renderQueue.CreateCameraEntry(vp.cameraProjection, vp.cameraTransform);
		_world.RenderID(vp.renderQueue, &vp.cameraFrustum);

		vp.BindFramebuffer();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_shaderIDMap.Use();
		vp.renderQueue.Render(ERenderChannels::SURFACE | ERenderChannels::UNLIT | (_drawEditorFeatures ? ERenderChannels::EDITOR : ERenderChannels::NONE), engine.pMeshManager, engine.pTextureManager, 0);
		GLFramebuffer::Unbind();

#if IDMAP_DEBUG
		{
			glViewport(bounds.x, bounds.y, bounds.w, bounds.h);

			_shaderUnlit.Use();
			RenderQueue t;
			RenderEntry& te = t.CreateEntry(ERenderChannels::UNLIT);
			te.AddSetMat4(RCMDSetMat4::Type::PROJECTION, Matrix4::ProjectionOrtho(-.5f, .5f, -.5f, .5f, -1.f, 1.f, 1.f));
			te.AddSetMat4(RCMDSetMat4::Type::VIEW, Matrix4::Identity());
			te.AddSetTransform(Matrix4::Transformation(Vector3(0.f, 0.f, -1.f), Quaternion(), Vector3(1.f, 1.f, 1.f)));
			te.AddSetColour(Colour::White);
			te.AddSetUVScale(Vector2(1.f, -1.f));
			te.AddSetUVOffset();
			te.AddSetTextureGL(vp.GetFramebufferTexGL(), 0);
			te.AddCommand(RCMDRenderMesh::PLANE);
			t.Render(ERenderChannels::UNLIT, engine.pMeshManager, engine.pTextureManager, 0);
		}
#endif
	}

	//Console
	if (_consoleWindow.IsVisible())
	{
		_consoleQueue.Clear();
		_consoleQueue.CreateCameraEntry(_consoleCamera, Transform(Vector3(_consoleCamera.GetDimensions().x / 2.f, _consoleCamera.GetDimensions().y / 2.f, 0.f)));
		engine.pConsole->Render(_consoleQueue, *_consoleFont, _deltaTime);

		_glContext.Use(_consoleWindow);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LEQUAL);

		_shaderUnlit.Use();
		_consoleQueue.Render(ERenderChannels::UNLIT, engine.pMeshManager, engine.pTextureManager, 0);
		_consoleWindow.SwapBuffers();
	}
}

void Editor::RenderViewport(Viewport& vp)
{
	float gridLimit = vp.cameraProjection.GetType() == EProjectionType::PERSPECTIVE ? 100.f : 0.f;

	const AbsoluteBounds& bounds = vp.ui.GetAbsoluteBounds();
	
	_shaderLit.Use();

	vp.renderQueue.Clear();

	if (cullingDebug)
		_viewports[0].cameraProjection.ToFrustum(_viewports[0].cameraTransform, vp.cameraFrustum);
	else
		vp.cameraProjection.ToFrustum(vp.cameraTransform, vp.cameraFrustum);

	//todo- wrap this up somewhere like other camera stuff?
	RenderEntry& cameraEntry = vp.renderQueue.CreateEntry(ERenderChannels::ALL);
	cameraEntry.AddSetViewport(bounds.x, bounds.y, vp.cameraProjection.GetDimensions().x, vp.cameraProjection.GetDimensions().y);
	cameraEntry.AddSetMat4(RCMDSetMat4::Type::PROJECTION, vp.cameraProjection.GetMatrix());
	cameraEntry.AddSetMat4(RCMDSetMat4::Type::VIEW, vp.cameraTransform.GetInverseMatrix());

	_world.Render(vp.renderQueue, &vp.cameraFrustum);

	if (_drawEditorFeatures)
	{
		RenderEntry& e = vp.renderQueue.CreateEntry(ERenderChannels::EDITOR);
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
		e.AddSetLineWidth(lineW);

		float z = vp.gridAxis == EAxis::Y ? _gridZ : 0.f;

		e.AddSetColour(Colour(.75f, .75f, .75f));
		e.AddGrid(vp.cameraTransform, vp.cameraProjection, vp.gridAxis, _gridUnit, gridLimit, 0.f, z);

		e.AddSetColour(Colour(.5f, .5f, 1.f));
		e.AddGrid(vp.cameraTransform, vp.cameraProjection, vp.gridAxis, 16.f, gridLimit, 0.f, z);

		e.AddSetColour(Colour(.5f, 1.f, 1.f));
		e.AddGrid(vp.cameraTransform, vp.cameraProjection, vp.gridAxis, 1000000.f, gridLimit, 0.f, z);

		engine.pDebugManager->RenderWorld(vp.renderQueue);
	}

	if (_currentTool) _currentTool->Render(vp.renderQueue);

	/////////
	_shaderLit.Use();
	_reflect.Bind(100);
	_shaderLit.SetInt(DefaultUniformVars::intCubemap, 100);
	_shaderLit.SetInt(DefaultUniformVars::intTextureDiffuse, 0);
	_shaderLit.SetInt(DefaultUniformVars::intTextureNormal, 1);
	_shaderLit.SetInt(DefaultUniformVars::intTextureSpecular, 2);
	_shaderLit.SetInt(DefaultUniformVars::intTextureReflection, 3);
	vp.renderQueue.Render(_litRenderChannels, engine.pMeshManager, engine.pTextureManager, _shaderLit.GetInt(DefaultUniformVars::intLightCount));

	_shaderUnlit.Use();
	vp.renderQueue.Render(_unlitRenderChannels | (_drawEditorFeatures ? ERenderChannels::EDITOR : ERenderChannels::NONE), engine.pMeshManager, engine.pTextureManager, 0);
}

void Editor::_RefreshVPs()
{
	for (int i = 0; i < VIEWPORTCOUNT; ++i)
	{
		const AbsoluteBounds& bounds = _viewports[i].ui.GetAbsoluteBounds();
		_viewports[i].Resize(Vector2T((uint16)bounds.w, (uint16)bounds.h));
	}
}

String Editor::SelectMaterialDialog()
{
	String string = ResourceSelect::Dialog(engine, "Data/Materials/*.txt", _window.GetHWND(),
		EResourceType::MATERIAL, _glContext, _shaderLit, _shaderUnlit);

	
	return string;
}

String Editor::SelectModelDialog()
{
	String string = ResourceSelect::Dialog(engine, "Data/Models/*.txt", _window.GetHWND(),
		EResourceType::MODEL, _glContext, _shaderLit, _shaderUnlit);

	
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

void Editor::ChangePropertyWorldObject(WorldObject* obj)
{
	ClearProperties();
	UIPropertyManipulator::AddPropertiesToContainer(1.f, PROPERTY_HEIGHT, *this, obj->GetProperties(), obj, _propertyContainer);
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
	case ETool::ISO:
		newTool = &tools.iso;
		break;
	case ETool::BRUSH2D:
		newTool = &tools.brush2D;
		break;
	case ETool::BRUSH3D:
		newTool = &tools.brush3D;
		break;
	case ETool::OBJECT:
		newTool = &tools.object;
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

void _CalcUnitXY(float x, float y, const Viewport& vp, float& unitX_out, float& unitY_out)
{
	Vector3 right = vp.cameraTransform.GetRightVector();
	Vector3 up = vp.cameraTransform.GetUpVector();

	switch (vp.GetCameraType())
	{
	case Viewport::ECameraType::ORTHO_X:
	case Viewport::ECameraType::ORTHO_Y:
	case Viewport::ECameraType::ORTHO_Z:
	{
		int rightElement = right.x ? 0 : right.y ? 1 : 2;
		int upElement = up.x ? 0 : up.y ? 1 : 2;
		unitX_out = vp.cameraTransform.GetPosition()[rightElement] + x / vp.cameraProjection.GetOrthographicScale();
		unitY_out = vp.cameraTransform.GetPosition()[upElement] + y / vp.cameraProjection.GetOrthographicScale();
	}
	break;

	case Viewport::ECameraType::ISOMETRIC:
	{
		Ray r = vp.cameraProjection.ScreenToWorld(vp.cameraTransform, Vector2(x / vp.cameraProjection.GetDimensions().x, y / vp.cameraProjection.GetDimensions().y));
		float t = Collision::IntersectRayPlane(r, Vector3(), Vector3(0.f, 1.f, 0.f));
		if (t > 0.f)
		{
			Vector3 pp = r.origin + r.direction * t;
			unitX_out = pp.x;
			unitY_out = pp.z;
		}
		else
			unitX_out = unitY_out = 0.f;
	}
	break;

	default:
		unitX_out = unitY_out = 0.f;
		break;
	}
}

void Editor::UpdateMousePosition(unsigned short x, unsigned short y)
{
	Viewport* vp = nullptr;

	for (int i = 0; i < VIEWPORTCOUNT; ++i)
	{
		const AbsoluteBounds& bounds = _viewports[i].ui.GetAbsoluteBounds();

		if (x >= bounds.x && y >= bounds.y &&
			x <= (bounds.x + bounds.w) &&
			y <= (bounds.y + bounds.h))
		{
			vp = &_viewports[i];
			break;
		}
	}

	if (vp == nullptr)
	{
		_mouseData.viewport = nullptr;
		return;
	}

	if (_mouseData.viewport != vp)
	{
		if (_mouseData.isLeftDown)
			LeftMouseUp();
	}

	const AbsoluteBounds& bounds = vp->ui.GetAbsoluteBounds();
	Viewport* prevViewport = _mouseData.viewport;

	_mouseData.prevX = _mouseData.x;
	_mouseData.prevY = _mouseData.y;
	_mouseData.prevUnitX = _mouseData.unitX;
	_mouseData.prevUnitY = _mouseData.unitY;
	_mouseData.viewport = vp;
	_mouseData.x = x - (uint16)(bounds.x + (bounds.w / 2.f));
	_mouseData.y = y - (uint16)(bounds.y + (bounds.h / 2.f));

	//Drag movement
	if (_mouseData.isRightDown && _activeVP)
	{
		if (_activeVP->cameraProjection.GetType() == EProjectionType::ORTHOGRAPHIC)
		{
			bool iso = _activeVP->GetCameraType() == Viewport::ECameraType::ISOMETRIC;
			Vector3 avpRight = _activeVP->cameraTransform.GetRightVector();
			Vector3 avpUp = _activeVP->cameraTransform.GetUpVector();
			int rightElement = iso ? 0 : (avpRight.x ? 0 : avpRight.y ? 1 : 2);
			int upElement = iso ? 2 : (avpUp.x ? 0 : avpUp.y ? 1 : 2);

			Vector3 duvec;
			duvec[rightElement] = _mouseData.dragUnitX;
			duvec[upElement] = _mouseData.dragUnitY;

			AbsoluteBounds avpBounds = _activeVP->ui.GetAbsoluteBounds();
			int avpX = x - (uint16)(avpBounds.x + (avpBounds.w / 2.f));
			int avpY = y - (uint16)(avpBounds.y + (avpBounds.h / 2.f));

			_activeVP->cameraTransform.SetPosition(Collision::ClosestPointOnPlane(_activeVP->cameraTransform.GetPosition(), _activeVP->cameraTransform.GetForwardVector(), duvec));
			_activeVP->cameraTransform.Move(avpRight * (-avpX / _activeVP->cameraProjection.GetOrthographicScale()) +
				avpUp * (-avpY / _activeVP->cameraProjection.GetOrthographicScale()));
		}
		else if (_mouseData.viewport == prevViewport)
		{
			_activeVP->cameraTransform.AddRotation(Vector3((_mouseData.y - _mouseData.prevY) * .5f, (_mouseData.x - _mouseData.prevX) * .5f, 0.f));
		}
	}

	//Update unitX and unitY
	_CalcUnitXY((float)_mouseData.x, (float)_mouseData.y, *_mouseData.viewport, _mouseData.unitX, _mouseData.unitY);
	_mouseData.unitX_rounded = _mouseData.unitX < 0.f ? (int)(_mouseData.unitX - 1.f) : (int)_mouseData.unitX;
	_mouseData.unitY_rounded = _mouseData.unitY < 0.f ? (int)(_mouseData.unitY - 1.f) : (int)_mouseData.unitY;

	_window.SetTitle(CSTR(_filename, " // Mouse X:", _mouseData.x, " (", _mouseData.unitX, ") Mouse Y:", _mouseData.y, " (", _mouseData.unitY, ')'));

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
	_mouseData.isLeftDown = false;

	if (_currentTool)
		_currentTool->MouseUp(_mouseData);
}

void Editor::RightMouseDown()
{
	_mouseData.isRightDown = true;
	_mouseData.dragUnitX = _mouseData.unitX;
	_mouseData.dragUnitY = _mouseData.unitY;

	if (_mouseData.viewport)
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

void Editor::IncreaseGridUnit()
{
	_gridUnit = Maths::Min(_gridUnit * 2.f, 8.f);
}

void Editor::DecreaseGridUnit()
{
	_gridUnit = Maths::Max(_gridUnit / 2.f, 1.f);
}

void Editor::ToggleConsole()
{
	_consoleIsActive = !_consoleIsActive;

	if (_consoleIsActive)
		_consoleWindow.Show();
	else
		_consoleWindow.Hide();
}

void Editor::Zoom(float amount)
{
	if (_mouseData.viewport)
	{
		Viewport& vp = *_mouseData.viewport;

		if (vp.cameraProjection.GetType() == EProjectionType::ORTHOGRAPHIC)
		{
			float mouseOffsetUnitsX = (float)_mouseData.x / vp.cameraProjection.GetOrthographicScale();
			float mouseOffsetUnitsY = (float)_mouseData.y / vp.cameraProjection.GetOrthographicScale();
			float moveX = mouseOffsetUnitsX - ((float)mouseOffsetUnitsX / (float)amount);
			float moveY = mouseOffsetUnitsY - ((float)mouseOffsetUnitsY / (float)amount);

			vp.cameraProjection.SetOrthographicScale(vp.cameraProjection.GetOrthographicScale() * amount);
			vp.cameraTransform.Move(vp.cameraTransform.GetRightVector() * moveX + vp.cameraTransform.GetUpVector() * moveY);
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
		[[fallthrough]];
	
	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK Editor::_WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui::GetCurrentContext())
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
			return TRUE;

		ImGuiIO& io = ImGui::GetIO();

		if (io.WantCaptureMouse && WindowFunctions_Win32::IsMouseInput(msg))
			return 0;
		if (io.WantCaptureKeyboard && WindowFunctions_Win32::IsKeyInput(msg))
			return 0;
	}

	Editor *editor = (Editor*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT create = (LPCREATESTRUCT)lparam;
		editor = (Editor*)create->lpCreateParams;
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)editor);

		::CheckMenuItem(::GetMenu(hwnd), ID_SHADING_UNLIT, MF_CHECKED);
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
				editor->_filename = IO::OpenFileDialog(L"\\Data\\Levels", levelDialogFilter);
				
				if (editor->_filename.GetLength())
				{
					editor->KeyCancel();
					editor->SetTool(ETool::SELECT);
					editor->_world.Clear();
					editor->_world.ReadObjects(editor->_filename.begin());

					::EnableMenuItem(::GetMenu(hwnd), ID_FILE_SAVE, MF_ENABLED);
				}
			}
			break;

			case ID_FILE_SAVE:
				if (editor->_filename.GetLength())
					editor->_world.WriteObjects(editor->_filename.begin());

				break;

			case ID_FILE_SAVEAS:
			{
				editor->_filename = IO::SaveFileDialog(L"\\Data\\Levels", levelDialogFilter);
				if (editor->_filename.GetLength())
				{
					editor->_world.WriteObjects(editor->_filename.begin());

					::EnableMenuItem(::GetMenu(hwnd), ID_FILE_SAVE, MF_ENABLED);
				}
			}
			break;

			case ID_IMPORT_ANIMATION:
			{
				Buffer<String> skeletalModelNames = editor->engine.pModelManager->GetAllPossibleKeys(editor->engine.context);

				for (size_t i = 0; i < skeletalModelNames.GetSize();)
				{
					SharedPointer<const Model> model = editor->engine.pModelManager->Get(skeletalModelNames[i], editor->engine.context);

					if (dynamic_cast<const Mesh_Skeletal*>(model->GetMesh().Ptr()))
						++i;
					else
						skeletalModelNames.Remove(i);
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

							Animation* animation = EditorIO::ReadFBXAnimation(editor->_fbxManager, filename.begin(), skeletalMesh->skeleton);

							if (animation)
							{
								String dest = IO::SaveFileDialog(L"\\Data\\Animations", saveAnimationFilter);
								if (dest.GetLength())
									IO::WriteFile(dest.begin(), animation->GetAsData());
								else
									Debug::Error("Okay, I guess you don't want to import an animation after all...");
							}

						}
					}
				}
			}
			break;

			case ID_IMPORT_MESH:
			{
				String filename = IO::OpenFileDialog(L"\\Data\\Meshes", openModelFilter);
				if (filename.GetLength() == 0)
					break;

				String ext = Utilities::GetExtension(filename).ToLower();

				Mesh *mesh = nullptr;

				if (Utilities::GetExtension(filename) == ".fbx")
				{
					mesh = EditorIO::ReadFBXMesh(editor->_fbxManager, filename.begin());
				}
				else if (Utilities::GetExtension(filename) == ".obj")
				{
					mesh = IO::ReadOBJFile(filename.begin());
				}
				else
				{
					Buffer<byte> data = IO::ReadFile(filename.begin());
					if (data.GetSize() <= 0)
						break;

					mesh = Mesh::FromData(data);
				}

				if (mesh && mesh->IsValid())
				{
					String dest = IO::SaveFileDialog(L"\\Data\\Meshes", saveModelFilter);

					if (dest.GetLength())
					{
						IO::WriteFile(dest.begin(), mesh->GetAsData());
					
						Mesh_Skeletal* skeletal = dynamic_cast<Mesh_Skeletal*>(mesh);
						if (skeletal && ::MessageBox(NULL, "Do you want to import the animation too?", "Hey", MB_YESNO) == IDYES)
						{
							Animation* anim = EditorIO::ReadFBXAnimation(editor->_fbxManager, filename.begin(), skeletal->skeleton);

							if (anim)
							{
								String animDest = IO::SaveFileDialog(L"\\Data\\Animations", saveAnimationFilter);

								if (animDest.GetLength())
								{
									IO::WriteFile(animDest.begin(), anim->GetAsData());
								}
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
				editor->_filename = "";
				editor->KeyCancel();
				editor->SetTool(ETool::SELECT);
				editor->_world.Clear();
				::EnableMenuItem(::GetMenu(hwnd), ID_FILE_SAVE, MF_GRAYED);
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

				::CheckMenuItem(::GetMenu(hwnd), ID_SHADING_UNLIT, MF_CHECKED);
				::CheckMenuItem(::GetMenu(hwnd), ID_SHADING_PHONG, MF_UNCHECKED);
				break;

			case ID_SHADING_PHONG:
				editor->_litRenderChannels = ERenderChannels::SURFACE;
				editor->_unlitRenderChannels = ERenderChannels::UNLIT;

				::CheckMenuItem(::GetMenu(hwnd), ID_SHADING_UNLIT, MF_UNCHECKED);
				::CheckMenuItem(::GetMenu(hwnd), ID_SHADING_PHONG, MF_CHECKED);
				break;

			case ID_DRAWEDITORFEATURES:
				editor->_drawEditorFeatures = !editor->_drawEditorFeatures;

				::CheckMenuItem(::GetMenu(hwnd), ID_DRAWEDITORFEATURES, editor->_drawEditorFeatures ? MF_CHECKED : MF_UNCHECKED);
				break;
			}
			break;
		}

		break;
	break;

	default:
		return WindowFunctions_Win32::WindowProc(&editor->_window, hwnd, msg, wparam, lparam);
	}

	return 0;
}

#pragma endregion
