#include "Editor.h"
#include <Engine/DrawUtils.h>
#include <Engine/IO.h>
#include <Engine/Ray.h>
#include <Engine/Renderable.h>
#include "EditorIO.h"
#include "resource.h"
#include "ResourceSelect.h"

constexpr int tbImageSize = 32;

constexpr GLfloat lineW = 1;
constexpr GLfloat lineW_Connector = 2;

const Buffer<Pair<const wchar_t*>> fdFilter({ Pair<const wchar_t*>(L"Level File", L"*.lvl"), Pair<const wchar_t*>(L"All Files", L"*.*") });

enum
{
	TBITEM_SELECT = 0,
	TBITEM_EDIT = 1,
	TBITEM_BRUSH = 2,
	TBITEM_ENTITY = 3,
	TBITEM_CONNECTOR = 4
};

#pragma region Init

void Editor::_Init()
{
	HBRUSH _windowBrush = ::CreateSolidBrush(RGB(32, 32, 32));

	{
		LPCTSTR classNameWindow = TEXT("MAINWINDOWCLASS");
		LPCTSTR classNameVPArea = TEXT("VPAREACLASS");

		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);
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

		_window.Create(classNameWindow, "Window", this);
		_vpArea.Create(classNameVPArea, NULL, this, WS_CHILD | WS_VISIBLE, _window.GetHwnd());
	}

	ResourceSelect::Initialise();

	PropertyWindow::Initialise(_windowBrush);
	_propertyWindow.Create(_window);

	ToolWindow::Initialise(_windowBrush);
	_toolWindow.Create(_window);

	Viewport::Initialise();
	for (int i = 0; i < VIEWPORTCOUNT; ++i)
		_viewports[i].Create(_vpArea.GetHwnd(), *this, i);

	_InitGL();

	CameraRef(0).SetProectionType(ProjectionType::PERSPECTIVE);
	CameraRef(0).transform.SetPosition(Vector3(-5.f, 5.f, -5.f));
	CameraRef(0).transform.SetRotation(Vector3(-45.f, 45.f, 0.f));

	CameraRef(1).SetProectionType(ProjectionType::ORTHOGRAPHIC);
	CameraRef(1).SetScale(32.f);
	CameraRef(1).SetZBounds(-10000.f, 10000.f);
	CameraRef(1).transform.SetRotation(Vector3(-90.f, 0.f, 0.f));

	CameraRef(2).SetProectionType(ProjectionType::ORTHOGRAPHIC);
	CameraRef(2).SetScale(32.f);
	CameraRef(2).SetZBounds(-10000.f, 10000.f);
	CameraRef(2).transform.SetRotation(Vector3(0.f, 0.f, 0.f));

	CameraRef(3).SetProectionType(ProjectionType::ORTHOGRAPHIC);
	CameraRef(3).SetScale(32.f);
	CameraRef(3).SetZBounds(-10000.f, 10000.f);
	CameraRef(3).transform.SetRotation(Vector3(0.f, -90.f, 0.f));


	_inputManager.BindKeyAxis(Keycode::W, &_axisMoveY, 1.f);
	_inputManager.BindKeyAxis(Keycode::S, &_axisMoveY, -1.f);
	_inputManager.BindKeyAxis(Keycode::D, &_axisMoveX, 1.f);
	_inputManager.BindKeyAxis(Keycode::A, &_axisMoveX, -1.f);

	_inputManager.BindKeyAxis(Keycode::UP, &_axisLookY, 1.f);
	_inputManager.BindKeyAxis(Keycode::DOWN, &_axisLookY, -1.f);
	_inputManager.BindKeyAxis(Keycode::RIGHT, &_axisLookX, 1.f);
	_inputManager.BindKeyAxis(Keycode::LEFT, &_axisLookX, -1.f);

	_inputManager.BindKey(Keycode::ENTER, *this, &Editor::KeySubmit);
	_inputManager.BindKey(Keycode::DEL, *this, &Editor::KeyDelete);

	_modelManager.Initialise();
	_modelManager.SetRootPath("Data/Models/");

	_textureManager.Initialise();
	_textureManager.SetRootPath("Data/Textures/");

	_materialManager.SetRootPath("Data/Materials/");

	//oof
	Engine::registry.RegisterEngineObjects();
	Engine::inputManager = &_inputManager;
	Engine::materialManager = &_materialManager;
	Engine::modelManager = &_modelManager;
	Engine::textureManager = &_textureManager;


	//Tool data init
	_tools.brush2D.Initialise();
	_tools.brush3D.Initialise();
	_tools.connector.Initialise();
	_tools.entity.Initialise();
	_tools.select.Initialise();
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

		_window.SetTitle(CSTR("Loading material \"" + filenames[i] + "\"..."));
		_materialManager.GetMaterial(filenames[i]);
	}

	//Preload models
	filenames = IO::FindFilesInDirectory("Data/Models/*.txt");
	for (uint32 i = 0; i < filenames.GetSize(); ++i)
	{
		Utilities::StripExtension(filenames[i]);

		_window.SetTitle(CSTR("Loading model \"" + filenames[i] + "\"..."));
		_modelManager.GetModel(filenames[i]);
	}

	_window.SetTitle("Editor");

	for (int i = 0; i < VIEWPORTCOUNT; ++i)
		_viewports[i].Show();

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
}

void Editor::Frame()
{
	const float moveSpeed = 4.f; // units/s
	const float rotSpeed = 90.f; //	degs/s

	_timer.Start();

	Camera &perspCam = CameraRef(0);
	perspCam.transform.Move(
		perspCam.transform.GetForwardVector() * _deltaTime * _axisMoveY * moveSpeed
		+ perspCam.transform.GetRightVector() * _deltaTime * _axisMoveX * moveSpeed);

	perspCam.transform.Rotate(Vector3(_deltaTime * _axisLookY * rotSpeed, _deltaTime * _axisLookX * rotSpeed, 0.f));

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
}

void Editor::RenderViewport(int index, Direction dir)
{
	auto camera = CameraRef(index);
	bool persp = camera.GetProjectionType() == ProjectionType::PERSPECTIVE;
	float boundsScale = persp ? 10.f : 1.f;

	_glContext.Use(_viewports[index]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LESS);

	_shaderUnlit.Use();
	_shaderUnlit.SetMat4(DefaultUniformVars::mat4Projection, camera.GetProjectionMatrix());
	_shaderUnlit.SetMat4(DefaultUniformVars::mat4View, camera.MakeInverseTransformationMatrix());
	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 1.f, 1.f, 1.f));
	_level.ObjectCollection().Render();

	if (!persp) glDepthFunc(GL_ALWAYS);
	
	glLineWidth(lineW);

	_textureManager.White().Bind(0);
	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(.75f, .75f, .75f, 1.f));
	DrawUtils::DrawGrid(_modelManager, camera, dir, 1.f, 1.f, boundsScale);

	if (persp) glDepthFunc(GL_LEQUAL);
	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(.5f, .5f, 1.f, 1.f));
	DrawUtils::DrawGrid(_modelManager, camera, dir, 1.f, 10.f, boundsScale);
	if (persp) glDepthFunc(GL_LESS);

	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(0.f, 1.f, 0.f, 1.f));

	glLineWidth(lineW_Connector);
	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(0.f, 1.f, 0.f, 1.f));
	for (uint32 i = 0; i < _level.Connectors().GetSize(); ++i)
		_level.Connectors()[i].Render();

	if (_currentTool) _currentTool->Render();

	_viewports[index].SwapBuffers();
}

void Editor::ResizeViews(uint16 w, uint16 h)
{
	const uint16 border = 4;
	uint16 vpW = (w - border) / 2;
	uint16 vpH = (h - border) / 2;

	_viewports[0].SetSizeAndPos(0, 0, vpW, vpH);
	_viewports[1].SetSizeAndPos(vpW + border, 0, vpW, vpH);
	_viewports[2].SetSizeAndPos(0, vpH + border, vpW, vpH);
	_viewports[3].SetSizeAndPos(vpW + border, vpH + border, vpW, vpH);

	glViewport(0, 0, vpW, vpH);

	Render();
}

String Editor::SelectMaterialDialog()
{
	String string = ResourceSelect::Dialog(_materialManager, _modelManager, "Data/Materials/*.txt", _propertyWindow.GetHwnd(),
		ResourceType::MATERIAL, _glContext, _shaderLit);

	RECT rect;
	::GetClientRect(_window.GetHwnd(), &rect);
	ResizeViews((uint16)rect.right, (uint16)rect.bottom);

	return string;
}

String Editor::SelectModelDialog()
{
	String string = ResourceSelect::Dialog(_materialManager, _modelManager, "Data/Models/*.txt", _propertyWindow.GetHwnd(),
		ResourceType::MODEL, _glContext, _shaderLit);

	RECT rect;
	::GetClientRect(_window.GetHwnd(), &rect);
	ResizeViews((uint16)rect.right, (uint16)rect.bottom);

	return string;
}

#pragma region Input

void Editor::UpdateMousePosition(int vpIndex, unsigned short x, unsigned short y)
{
	Camera& camera = CameraRef(vpIndex);
	Vector3 right = camera.transform.GetRightVector();
	Vector3 up = camera.transform.GetUpVector();

	if (camera.GetProjectionType() == ProjectionType::PERSPECTIVE)
		_mouseData.rightElement = _mouseData.upElement = _mouseData.forwardElement = 0;
	else
	{
		_mouseData.rightElement = right[0] ? 0 : right[1] ? 1 : 2;
		_mouseData.upElement = up[0] ? 0 : up[1] ? 1 : 2;
		_mouseData.forwardElement = (_mouseData.rightElement != 0 && _mouseData.upElement != 0) ? 0 : (_mouseData.rightElement != 1 && _mouseData.upElement != 1) ? 1 : 2;
	}

	_mouseData.viewport = vpIndex;
	_mouseData.x = x - (camera.GetViewport()[0] / 2);
	_mouseData.y = -(y - (camera.GetViewport()[1] / 2));
	_mouseData.unitX = camera.transform.Position()[_mouseData.rightElement] + (float)_mouseData.x / camera.GetScale();
	_mouseData.unitY = camera.transform.Position()[_mouseData.upElement] + (float)_mouseData.y / camera.GetScale();
	_mouseData.unitX_rounded = _mouseData.unitX < 0.f ? (int)(_mouseData.unitX - 1.f) : (int)_mouseData.unitX;
	_mouseData.unitY_rounded = _mouseData.unitY < 0.f ? (int)(_mouseData.unitY - 1.f) : (int)_mouseData.unitY;

	if (camera.GetProjectionType() == ProjectionType::ORTHOGRAPHIC)
		_window.SetTitle(CSTR(String::FromInt(_mouseData.viewport) + " Mouse X:" + String::FromInt(_mouseData.x) + " (" + String::FromFloat(_mouseData.unitX, 0, 2) + " ) Mouse Y:" + String::FromInt(_mouseData.y) + " (" + String::FromFloat(_mouseData.unitY, 0, 2) + ')'));
	else
		_window.SetTitle(CSTR(String::FromInt(_mouseData.viewport) + " Mouse X:" + String::FromInt(_mouseData.x) + " Mouse Y:" + String::FromInt(_mouseData.y)));

	if (_currentTool)
		_currentTool->MouseMove(_mouseData);

	_propertyWindow.Refresh();
}

void Editor::LeftMouseDown()
{
	::SetFocus(_window.GetHwnd());

	_mouseData.isLeftDown = true;
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

void Editor::KeySubmit()
{
	if (_currentTool)
		_currentTool->KeySubmit();
}

void Editor::KeyDelete()
{
	if (_currentTool)
		_currentTool->KeyDelete();
}

void Editor::Zoom(float amount)
{
	Camera& camera = CameraRef(_mouseData.viewport);

	if (camera.GetProjectionType() == ProjectionType::ORTHOGRAPHIC)
	{
		float mouseOffsetUnitsX = (float)_mouseData.x / camera.GetScale();
		float mouseOffsetUnitsY = (float)_mouseData.y / camera.GetScale();
		float moveX = mouseOffsetUnitsX - ((float)mouseOffsetUnitsX / (float)amount);
		float moveY = mouseOffsetUnitsY - ((float)mouseOffsetUnitsY / (float)amount);

		camera.SetScale(camera.GetScale() * amount);
		camera.transform.Move(camera.transform.GetRightVector() * moveX + camera.transform.GetUpVector() * moveY);
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
			{0, TBITEM_SELECT,  TBSTATE_ENABLED, btnStyle, {0}, 0, (INT_PTR)TEXT("Select") },
			{1, TBITEM_EDIT, TBSTATE_ENABLED, btnStyle, {0}, 0, (INT_PTR)TEXT("Edit")},
			{2, TBITEM_BRUSH, TBSTATE_ENABLED, btnStyle, {0}, 0, (INT_PTR)TEXT("Brush")},
			{3, TBITEM_ENTITY, TBSTATE_ENABLED, btnStyle, {0}, 0, (INT_PTR)TEXT("Entity")},
			{4, TBITEM_CONNECTOR, TBSTATE_ENABLED, btnStyle, {0}, 0, (INT_PTR)TEXT("Connector")}
		};

		SendMessage(editor->_toolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
		SendMessage(editor->_toolbar, TB_ADDBUTTONS, (WPARAM)5, (LPARAM)&tbButtons);

		SendMessage(editor->_toolbar, TB_AUTOSIZE, 0, 0);
		SendMessage(editor->_toolbar, TB_SETSTATE, TBITEM_EDIT, MAKELONG(TBSTATE_ENABLED | TBSTATE_CHECKED, 0));
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
				char cd[MAX_PATH];
				::GetCurrentDirectoryA(MAX_PATH, cd);
				String filename = EditorIO::OpenFileDialog(L"\\Data\\Levels", fdFilter);
				editor->_level.Clear();
				editor->_level.ReadFromFile(filename.GetData());
			}
			break;

			case ID_FILE_SAVEAS:
			{
				char cd[MAX_PATH];
				::GetCurrentDirectoryA(MAX_PATH, cd);
				String filename = EditorIO::SaveFileDialog(L"\\Data\\Levels", fdFilter);
				editor->_level.WriteToFile(filename.GetData());
			}
			break;

			case ID_FILE_CLEAR:
				editor->_level.Clear();
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

#define SETTOOL(NAME) editor->_currentTool = &editor->_tools.NAME; editor->_currentTool->Activate(editor->_propertyWindow, editor->_toolWindow.PropertyWindow())

			case TBITEM_SELECT:	SETTOOL(select);	break;
			case TBITEM_EDIT:	SETTOOL(brush2D);	break;
			case TBITEM_BRUSH:	SETTOOL(brush3D);	break;
			case TBITEM_ENTITY:	SETTOOL(entity);	break;
			case TBITEM_CONNECTOR: SETTOOL(connector); break;
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
		const int propertiesW = 256;
		const int toolbarH = 64;
		const int toolWindowH = 256;

		editor->_vpArea.SetSizeAndPos(0, toolbarH, w - propertiesW, h - toolbarH);
		editor->_propertyWindow.SetSizeAndPos(w - propertiesW, toolbarH, propertiesW, h - toolbarH - toolWindowH);
		editor->_toolWindow.SetSizeAndPos(w - propertiesW, h - toolWindowH, propertiesW, toolWindowH);
		WindowFunctions::SetHWNDSizeAndPos(editor->_toolbar, 0, 0, w, toolbarH);
	}
	break;

	case WM_KEYDOWN:
		if (lparam & (1 << 30))
			break; //Key repeats ignored

		editor->_inputManager.KeyDown((Keycode)wparam);
		break;
	case WM_KEYUP:

		editor->_inputManager.KeyUp((Keycode)wparam);
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
