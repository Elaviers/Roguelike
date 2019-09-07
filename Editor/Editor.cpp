#include "Editor.hpp"
#include <Engine/DebugFrustum.hpp>
#include <Engine/DebugManager.hpp>
#include <Engine/DrawUtils.hpp>
#include <Engine/InputManager.hpp>
#include <Engine/IO.hpp>
#include <Engine/ObjLight.hpp>
#include <Engine/ObjRenderable.hpp>
#include <Engine/Ray.hpp>
#include "EditorIO.hpp"
#include "resource.h"
#include "ResourceSelect.hpp"
#include "StringSelect.hpp"

constexpr int tbImageSize = 32;

constexpr GLfloat lineW = 1;

const Buffer<Pair<const wchar_t*>> levelDialogFilter({ Pair<const wchar_t*>(L"Level File", L"*.lvl"), Pair<const wchar_t*>(L"All Files", L"*.*") });

const Buffer<Pair<const wchar_t*>> openAnimationFilter({Pair<const wchar_t*>(L"FBX Scene", L"*.fbx")});
const Buffer<Pair<const wchar_t*>> openModelFilter({Pair<const wchar_t*>(L"FBX Scene", L"*.fbx"), Pair<const wchar_t*>(L"OBJ Model", L"*.obj")});
const Buffer<Pair<const wchar_t*>> openTextureFilter({Pair<const wchar_t*>(L"PNG Image", L"*.png")});

const Buffer<Pair<const wchar_t*>> saveAnimationFilter({ Pair<const wchar_t*>(L"Animation", L"*.anim") });
const Buffer<Pair<const wchar_t*>> saveModelFilter({Pair<const wchar_t*>(L"Mesh", L"*.mesh")});

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
	LPCTSTR classNameWindow = TEXT("MAINWINDOWCLASS");
	LPCTSTR classNameVPArea = TEXT("VPAREACLASS");

	GLContext dummy;

	{
		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);

		windowClass.lpfnWndProc = ::DefWindowProc;
		windowClass.hInstance = ::GetModuleHandle(NULL);
		windowClass.lpszClassName = classNameDummy;
		::RegisterClassEx(&windowClass);

		windowClass.lpfnWndProc = _WindowProc;
		windowClass.hInstance = ::GetModuleHandle(NULL);
		windowClass.hbrBackground = _windowBrush;
		windowClass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
		windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = classNameWindow;
		windowClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
		::RegisterClassEx(&windowClass);

		windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		windowClass.lpszClassName = classNameVPArea;
		windowClass.lpfnWndProc = _vpAreaProc;
		::RegisterClassEx(&windowClass);

		dummy.CreateDummyAndUse(classNameDummy);
		GL::LoadDummyExtensions();

		_window.Create(classNameWindow, "Window", this);
		_vpArea.Create(classNameVPArea, NULL, this, WS_CHILD | WS_VISIBLE, _window.GetHwnd());
	}

	ResourceSelect::Initialise();

	HierachyWindow::Initialise(_windowBrush);
	_hierachyWindow.Create(&_window);

	PropertyWindow::Initialise(_windowBrush);
	_propertyWindow.Create(_window);

	ToolWindow::Initialise(_windowBrush);
	_toolWindow.Create(_window);

	Viewport::Initialise();
	for (int i = 0; i < VIEWPORTCOUNT; ++i)
		_viewports[i].Create(_vpArea.GetHwnd(), *this, i);

	//Now we can make a real context
	dummy.Delete();
	_InitGL();

	_uiCam.SetProjectionType(ProjectionType::ORTHOGRAPHIC);
	_uiCam.SetZBounds(-10, 10);
	_uiCam.SetScale(1.f);

	CameraRef(0).SetProjectionType(ProjectionType::PERSPECTIVE);
	CameraRef(0).SetRelativePosition(Vector3(-5.f, 5.f, -5.f));
	CameraRef(0).SetRelativeRotation(Vector3(-45.f, 45.f, 0.f));

	CameraRef(1).SetProjectionType(ProjectionType::ORTHOGRAPHIC);
	CameraRef(1).SetScale(32.f);
	CameraRef(1).SetZBounds(-10000.f, 10000.f);
	CameraRef(1).SetRelativeRotation(Vector3(-90.f, 0.f, 0.f));

	CameraRef(2).SetProjectionType(ProjectionType::ORTHOGRAPHIC);
	CameraRef(2).SetScale(32.f);
	CameraRef(2).SetZBounds(-10000.f, 10000.f);
	CameraRef(2).SetRelativeRotation(Vector3(0.f, 0.f, 0.f));

	CameraRef(3).SetProjectionType(ProjectionType::ORTHOGRAPHIC);
	CameraRef(3).SetScale(32.f);
	CameraRef(3).SetZBounds(-10000.f, 10000.f);
	CameraRef(3).SetRelativeRotation(Vector3(0.f, -90.f, 0.f));

	_fbxManager = FbxManager::Create();
	Engine::Instance().Init(ENG_ALL);

	InputManager* inputManager = Engine::Instance().pInputManager;

	inputManager->BindKeyAxis(Keycode::W, &_axisMoveY, 1.f);
	inputManager->BindKeyAxis(Keycode::S, &_axisMoveY, -1.f);
	inputManager->BindKeyAxis(Keycode::D, &_axisMoveX, 1.f);
	inputManager->BindKeyAxis(Keycode::A, &_axisMoveX, -1.f);
	inputManager->BindKeyAxis(Keycode::SPACE, &_axisMoveZ, 1.f);
	inputManager->BindKeyAxis(Keycode::SHIFT, &_axisMoveZ, -1.f);

	inputManager->BindKeyAxis(Keycode::UP, &_axisLookY, 1.f);
	inputManager->BindKeyAxis(Keycode::DOWN, &_axisLookY, -1.f);
	inputManager->BindKeyAxis(Keycode::RIGHT, &_axisLookX, 1.f);
	inputManager->BindKeyAxis(Keycode::LEFT, &_axisLookX, -1.f);

	inputManager->BindKey(Keycode::ENTER, *this, &Editor::KeySubmit);
	inputManager->BindKey(Keycode::ESCAPE, *this, &Editor::KeyCancel);
	inputManager->BindKey(Keycode::DEL, *this, &Editor::KeyDelete);

	inputManager->BindKey(Keycode::TILDE, *this, &Editor::RefreshLevel);

	_level.onNameChanged += Callback(this, &Editor::RefreshLevel);
	_level.onChildChanged += Callback(this, &Editor::RefreshLevel);

	//Tool data init
	tools.brush2D.Initialise();
	tools.brush3D.Initialise();
	tools.connector.Initialise();
	tools.entity.Initialise();
	tools.select.Initialise();

	SetTool(TOOL_SELECT);
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

	_shaderLit.Load("Data/Shaders/Shader.vert", "Data/Shaders/Phong.frag");
	_shaderUnlit.Load("Data/Shaders/Shader.vert", "Data/Shaders/Unlit.frag");
}

#pragma endregion

void Editor::Run()
{
	_Init();
	_window.Show();

	//Preload materials
	Buffer<String> filenames = IO::FindFilesInDirectory("Data/Materials/*.txt");
	for (uint32 i = 0; i < filenames.GetSize(); ++i)
	{
		Utilities::StripExtension(filenames[i]);

		_window.SetTitle(CSTR("Loading material \"", filenames[i], "\"..."));
		Engine::Instance().pMaterialManager->Get(filenames[i]);
	}

	//Preload models
	filenames = IO::FindFilesInDirectory("Data/Models/*.txt");
	for (uint32 i = 0; i < filenames.GetSize(); ++i)
	{
		Utilities::StripExtension(filenames[i]);

		_window.SetTitle(CSTR("Loading model \"", filenames[i], "\"..."));
		Engine::Instance().pModelManager->Get(filenames[i]);
	}

	_window.SetTitle("Editor");

	for (int i = 0; i < VIEWPORTCOUNT; ++i)
		_viewports[i].Show();

	_hierachyWindow.Show();

	_propertyWindow.Show();

	_toolWindow.Show();
	_toolWindow.PropertyWindow().Show();

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

	::DeleteObject(_windowBrush);

	_glContext.Delete();
}

void Editor::Frame()
{
	const float moveSpeed = 4.f; // units/s
	const float rotSpeed = 90.f; //	degs/s

	_timer.Start();

	ObjCamera &perspCam = CameraRef(_activeVP);
	perspCam.RelativeMove(
		perspCam.GetRelativeTransform().GetForwardVector() * _deltaTime * _axisMoveY * moveSpeed
		+ perspCam.GetRelativeTransform().GetRightVector() * _deltaTime * _axisMoveX * moveSpeed
		+ perspCam.GetRelativeTransform().GetUpVector() * _deltaTime * _axisMoveZ * moveSpeed);

	perspCam.AddRelativeRotation(Vector3(_deltaTime * _axisLookY * rotSpeed, _deltaTime * _axisLookX * rotSpeed, 0.f));

	Engine::Instance().pDebugManager->Update(_deltaTime);
	Engine::Instance().pDebugManager->AddToWorld(DebugFrustum::FromCamera(CameraRef(0)));

	Render();

	_deltaTime = _timer.SecondsSinceStart();
}

void Editor::Render()
{
	if (!_glContext.IsValid())
		return;

	RenderViewport(0, Direction::UP);
	RenderViewport(1, Direction::UP);
	RenderViewport(2, Direction::FORWARD);
	RenderViewport(3, Direction::RIGHT);

	_shaderUnlit.Use();
	_uiCam.Use();
	Engine::Instance().pDebugManager->RenderScreen();
}

void Editor::RenderViewport(int index, Direction dir)
{
	auto& camera = CameraRef(index);
	bool persp = camera.GetProjectionType() == ProjectionType::PERSPECTIVE;
	float gridLimit = persp ? 100.f : 0.f;

	_glContext.Use(_viewports[index]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);
	
	{	//LIT PASS
		_shaderLit.Use();
		camera.Use();
		_shaderLit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 1.f, 1.f, 1.f));

		_shaderLit.SetInt(DefaultUniformVars::intTextureDiffuse, 0);
		_shaderLit.SetInt(DefaultUniformVars::intTextureNormal, 1);
		_shaderLit.SetInt(DefaultUniformVars::intTextureSpecular, 2);
		_shaderLit.SetInt(DefaultUniformVars::intTextureReflection, 3);

		_shaderLit.SetVec2(DefaultUniformVars::vec2UVOffset, Vector2());
		_shaderLit.SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1, 1));

		Buffer<ObjLight*> lights = _level.FindChildrenOfType<ObjLight>(true);

		for (size_t i = 0; i < 8; ++i)
		{
			if (i < lights.GetSize())
				lights[i]->ToShader((int)i);
			else
			{
				float zero = 0.f;
				glUniform1fv(GLProgram::Current().GetUniformLocation(CSTR("Lights[", i, "].Radius")), 1, &zero);
			}
		}

		_level.Render(CameraRef(0), _litRenderChannels);
	}

	//UNLIT PASS
	_shaderUnlit.Use();
	camera.Use();
	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 1.f, 1.f, 1.f));

	_level.Render(CameraRef(0), EnumRenderChannel(_unlitRenderChannels | (_drawEditorFeatures ? RenderChannel::EDITOR : 0)));
	
	if (_drawEditorFeatures)
	{
		if (!persp) glDepthFunc(GL_ALWAYS);

		glLineWidth(lineW);

		Engine::Instance().pTextureManager->White()->Bind(0);
		_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(.75f, .75f, .75f, 1.f));
		DrawUtils::DrawGrid(*Engine::Instance().pModelManager, camera, dir, 1.f, 1.f, gridLimit);

		if (persp) glDepthFunc(GL_LEQUAL);
		_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(.5f, .5f, 1.f, 1.f));
		DrawUtils::DrawGrid(*Engine::Instance().pModelManager, camera, dir, 1.f, 10.f, gridLimit);

		Engine::Instance().pDebugManager->RenderWorld();
	}

	Engine::Instance().pTextureManager->White()->Bind(0);
	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 1.f, 1.f, 1.f));
	if (_currentTool) _currentTool->Render(RenderChannel::ALL);

	glDepthFunc(GL_ALWAYS);
	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 1.f, 1.f, 1.f));
	_level.Render(CameraRef(0), RenderChannel::SPRITE);
	glDepthFunc(GL_LEQUAL);

	_viewports[index].SwapBuffers();
}

void Editor::ResizeViews(uint16 w, uint16 h)
{
	const uint16 border = 4;
	uint16 vpW = (w - border) / 2;
	uint16 vpH = (h - border) / 2;

	_uiCam.SetViewport(w, h);

	_viewports[0].SetSizeAndPos(0, 0, vpW, vpH);
	_viewports[1].SetSizeAndPos(vpW + border, 0, vpW, vpH);
	_viewports[2].SetSizeAndPos(0, vpH + border, vpW, vpH);
	_viewports[3].SetSizeAndPos(vpW + border, vpH + border, vpW, vpH);

	glViewport(0, 0, vpW, vpH);

	Render();
}

String Editor::SelectMaterialDialog()
{
	String string = ResourceSelect::Dialog(*Engine::Instance().pMaterialManager, *Engine::Instance().pModelManager, "Data/Materials/*.txt", _propertyWindow.GetHwnd(),
		ResourceType::MATERIAL, _glContext, _shaderLit, _shaderUnlit);

	RECT rect;
	::GetClientRect(_window.GetHwnd(), &rect);
	ResizeViews((uint16)rect.right, (uint16)rect.bottom);

	return string;
}

String Editor::SelectModelDialog()
{
	String string = ResourceSelect::Dialog(*Engine::Instance().pMaterialManager, *Engine::Instance().pModelManager, "Data/Models/*.txt", _propertyWindow.GetHwnd(),
		ResourceType::MODEL, _glContext, _shaderLit, _shaderUnlit);

	RECT rect;
	::GetClientRect(_window.GetHwnd(), &rect);
	ResizeViews((uint16)rect.right, (uint16)rect.bottom);

	return string;
}

void Editor::SetTool(ToolEnum tool, bool changeToolbar)
{
	if (changeToolbar)
		SendMessage(_toolbar, TB_CHECKBUTTON, tool, MAKELONG(TRUE, 0));

	Tool* newTool = nullptr;

	switch (tool)
	{
	case TOOL_SELECT:
		newTool = &tools.select;
		break;
	case TOOL_BRUSH2D:
		newTool = &tools.brush2D;
		break;
	case TOOL_BRUSH3D:
		newTool = &tools.brush3D;
		break;
	case TOOL_ENTITY:
		newTool = &tools.entity;
		break;
	case TOOL_CONNECTOR:
		newTool = &tools.connector;
		break;
	}

	if (_currentTool)
		_currentTool->Deactivate();

	_currentTool = newTool;
	_currentTool->Activate(_propertyWindow, _toolWindow.PropertyWindow());
}

#pragma region Input

void Editor::UpdateMousePosition(int vpIndex, unsigned short x, unsigned short y)
{
	if (_mouseData.viewport != vpIndex)
	{
		if (_mouseData.isLeftDown)
			LeftMouseUp();

		if (_mouseData.isRightDown)
			RightMouseUp();
	}

	ObjCamera& camera = CameraRef(vpIndex);
	Vector3 right = camera.GetRelativeTransform().GetRightVector();
	Vector3 up = camera.GetRelativeTransform().GetUpVector();

	if (camera.GetProjectionType() == ProjectionType::PERSPECTIVE)
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
	_mouseData.x = x - (camera.GetViewport()[0] / 2);
	_mouseData.y = -(y - (camera.GetViewport()[1] / 2));
	_mouseData.unitX = camera.GetRelativePosition()[_mouseData.rightElement] + (float)_mouseData.x / camera.GetScale();
	_mouseData.unitY = camera.GetRelativePosition()[_mouseData.upElement] + (float)_mouseData.y / camera.GetScale();

	if (_mouseData.isRightDown && _mouseData.viewport == prevViewport)
	{
		if (camera.GetProjectionType() == ProjectionType::ORTHOGRAPHIC)
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

	if (camera.GetProjectionType() == ProjectionType::ORTHOGRAPHIC)
		_window.SetTitle(CSTR(_mouseData.viewport, " Mouse X:", _mouseData.x, " (", _mouseData.unitX, " ) Mouse Y:", _mouseData.y, " (", _mouseData.unitY, ')'));
	else
		_window.SetTitle(CSTR(_mouseData.viewport, " Mouse X:", _mouseData.x, " (", _mouseData.unitX, " ) Mouse Y:", _mouseData.y));

	if (_currentTool)
		_currentTool->MouseMove(_mouseData);

	_propertyWindow.Refresh();
}

void Editor::LeftMouseDown()
{
	::SetFocus(_window.GetHwnd());

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

	if (CameraRef(_mouseData.viewport).GetProjectionType() == ProjectionType::PERSPECTIVE)
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

void Editor::RefreshLevel()
{
	_hierachyWindow.Refresh(_level);
}

void Editor::Zoom(float amount)
{
	ObjCamera& camera = CameraRef(_mouseData.viewport);

	if (camera.GetProjectionType() == ProjectionType::ORTHOGRAPHIC)
	{
		float mouseOffsetUnitsX = (float)_mouseData.x / camera.GetScale();
		float mouseOffsetUnitsY = (float)_mouseData.y / camera.GetScale();
		float moveX = mouseOffsetUnitsX - ((float)mouseOffsetUnitsX / (float)amount);
		float moveY = mouseOffsetUnitsY - ((float)mouseOffsetUnitsY / (float)amount);

		camera.SetScale(camera.GetScale() * amount);
		camera.RelativeMove(camera.GetRelativeTransform().GetRightVector() * moveX + camera.GetRelativeTransform().GetUpVector() * moveY);
	}

}

#pragma endregion

#pragma region WindowsProcs

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

		HINSTANCE instance = ::GetModuleHandle(NULL);

		editor->_tbImages = ::ImageList_Create(tbImageSize, tbImageSize, ILC_COLOR16 | ILC_MASK, 5, 0);

		HBITMAP bmp = ::LoadBitmap(instance, MAKEINTRESOURCE(IDB_TOOLSELECT));
		::ImageList_AddMasked(editor->_tbImages, bmp, RGB(255, 255, 255));
		::DeleteObject(bmp);

		bmp = ::LoadBitmap(instance, MAKEINTRESOURCE(IDB_TOOLEDIT));
		::ImageList_AddMasked(editor->_tbImages, bmp, RGB(255, 255, 255));
		::DeleteObject(bmp);

		bmp = ::LoadBitmap(instance, MAKEINTRESOURCE(IDB_TOOLBRUSH));
		ImageList_AddMasked(editor->_tbImages, bmp, RGB(255, 255, 255));
		::DeleteObject(bmp);

		bmp = ::LoadBitmap(instance, MAKEINTRESOURCE(IDB_TOOLENTITY));
		::ImageList_AddMasked(editor->_tbImages, bmp, RGB(255, 255, 255));
		::DeleteObject(bmp);

		bmp = ::LoadBitmap(instance, MAKEINTRESOURCE(IDB_TOOLCONNECTOR));
		::ImageList_AddMasked(editor->_tbImages, bmp, RGB(255, 255, 255));
		::DeleteObject(bmp);

		editor->_toolbar = ::CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, instance, NULL);
		::SendMessage(editor->_toolbar, TB_SETIMAGELIST, 0, (LPARAM)editor->_tbImages);

		BYTE btnStyle = BTNS_AUTOSIZE | BTNS_SHOWTEXT | BTNS_CHECKGROUP;

		TBBUTTON tbButtons[] =
		{
			{0, TOOL_SELECT,  TBSTATE_ENABLED, btnStyle, {0}, 0, (INT_PTR)TEXT("Select") },
			{1, TOOL_BRUSH2D, TBSTATE_ENABLED, btnStyle, {0}, 0, (INT_PTR)TEXT("Edit")},
			{2, TOOL_BRUSH3D, TBSTATE_ENABLED, btnStyle, {0}, 0, (INT_PTR)TEXT("Brush")},
			{3, TOOL_ENTITY, TBSTATE_ENABLED, btnStyle, {0}, 0, (INT_PTR)TEXT("Entity")},
			{4, TOOL_CONNECTOR, TBSTATE_ENABLED, btnStyle, {0}, 0, (INT_PTR)TEXT("Connector")}
		};

		SendMessage(editor->_toolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
		SendMessage(editor->_toolbar, TB_ADDBUTTONS, (WPARAM)5, (LPARAM)&tbButtons);

		SendMessage(editor->_toolbar, TB_AUTOSIZE, 0, 0);
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
				String filename = EditorIO::OpenFileDialog(L"\\Data\\Levels", levelDialogFilter);
				editor->KeyCancel();
				editor->_level.DeleteChildren();
				LevelIO::Read(editor->_level, filename.GetData());
			}
			break;

			case ID_FILE_SAVEAS:
			{
				String filename = EditorIO::SaveFileDialog(L"\\Data\\Levels", levelDialogFilter);
				LevelIO::Write(editor->_level, filename.GetData());
			}
			break;

			case ID_IMPORT_ANIMATION:
			{
				String filename = EditorIO::OpenFileDialog(L"\\Data\\Animations", openAnimationFilter);
				if (filename.GetLength() == 0)
					break;
				
				Animation* animation = EditorIO::ReadFBXAnimation(editor->_fbxManager, filename.GetData());

				if (animation)
				{
					String dest = EditorIO::SaveFileDialog(L"\\Data\\Animations", saveAnimationFilter);
					if (dest.GetLength())
						IO::WriteFile(dest.GetData(), animation->GetAsData());
					else
						Debug::Error("Okay, I guess you don't want to import an animation after all...");
				}
			}
			break;

			case ID_IMPORT_MODEL:
			{
				String filename = EditorIO::OpenFileDialog(L"\\Data\\Models", openModelFilter);
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
					String dest = EditorIO::SaveFileDialog(L"\\Data\\Models", saveModelFilter);

					if (dest.GetLength())
						IO::WriteFile(dest.GetData(), mesh->GetAsData());
					else
						Debug::Error("Umm.. I guess you don't want that model imported then.");
				}
				else
					Debug::Error("Sorry, but that ain't a supported model");
			}
			break;

			case ID_IMPORT_TEXTURE:
			{
				String filename = EditorIO::OpenFileDialog(L"\\Data\\Textures", openTextureFilter);

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
				editor->_litRenderChannels = RenderChannel::NONE;
				editor->_unlitRenderChannels = EnumRenderChannel(RenderChannel::SURFACE | RenderChannel::UNLIT);
				break;

			case ID_SHADING_PHONG:
				editor->_litRenderChannels = RenderChannel::SURFACE;
				editor->_unlitRenderChannels = RenderChannel::UNLIT;
				break;

			case ID_DRAWEDITORFEATURES:
				editor->_drawEditorFeatures = !editor->_drawEditorFeatures;

				::CheckMenuItem(::GetMenu(hwnd), ID_DRAWEDITORFEATURES, editor->_drawEditorFeatures ? MF_CHECKED : MF_UNCHECKED);
				break;


			case TOOL_SELECT:
			case TOOL_BRUSH2D:
			case TOOL_BRUSH3D:
			case TOOL_ENTITY:
			case TOOL_CONNECTOR:
				editor->SetTool((ToolEnum)LOWORD(wparam), false);
				break;
			}
			break;
		}

		break;

	case WM_CLOSE:
		::DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		::ImageList_Destroy(editor->_tbImages);

		editor->_running = false;
		break;

	case WM_SIZE:
	{
		uint16 w = LOWORD(lparam);
		uint16 h = HIWORD(lparam);
		const int leftW = 256;
		const int rightW = 256;
		const int topH = 64;
		const int toolWindowH = 256;

		editor->_vpArea.SetSizeAndPos(leftW, topH, w - leftW - rightW, h - topH);
		editor->_propertyWindow.SetSizeAndPos(w - rightW, topH, rightW, h - topH - toolWindowH);
		editor->_hierachyWindow.SetSizeAndPos(0, topH, leftW, h - topH);
		editor->_toolWindow.SetSizeAndPos(w - rightW, h - toolWindowH, rightW, toolWindowH);
		WindowFunctions::SetHWNDSizeAndPos(editor->_toolbar, 0, 0, w, topH);
	}
	break;

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		if (lparam & (1 << 30))
			break; //Key repeats ignored

		Engine::Instance().pInputManager->KeyDown((Keycode)wparam);
		break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		Engine::Instance().pInputManager->KeyUp((Keycode)wparam);
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
		break;

	case WM_MOUSEWHEEL:
		if ((signed short)HIWORD(wparam) > 0)
			editor->Zoom(1.25f);
		else
			editor->Zoom(.75f);
		break;

	default: return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

#pragma endregion

/*
	//UNUSED: HASHMAP TESTER

	Hashmap<String, int> test;

	String alph = IO::ReadFileString("file.txt");
	Buffer<String> lines = alph.Split("\r\n");

	int inc = 0;
	for (size_t i = 0; i < lines.GetSize(); ++i)
		test[lines[i]] = inc++;

	int d = 0;
	while (true)
	{
		auto b = test.ToKVBuffer(d);

		if (b.GetSize() == 0)
			break;


		Debug::Print(CSTR("\nDepth " + String::FromInt(d) + " : "));

		for (int i = 0; i < b.GetSize(); ++i)
		{
			auto keys = b[i];

			Debug::Print(CSTR(String::FromInt(Hasher<uint32>::Hash<String>(keys->First()->obj.first)) + ": {"));

			for (auto node = keys->First(); node; node = node->next)
			{
				Debug::Print(CSTR('\'' + node->obj.first + "' : " + String::FromInt(node->obj.second) + "  "));
			}

			Debug::Print("} ");
		}
		++d;
	}
	*/
