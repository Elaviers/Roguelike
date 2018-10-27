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
constexpr GLfloat lineW_ConnectorSelected = 3;

Vector3 rcp1, rcp2; //For raycast debug

const Buffer<Pair<const wchar_t*>> fdFilter({ Pair<const wchar_t*>(L"Level File", L"*.lvl"), Pair<const wchar_t*>(L"All Files", L"*.*") });

enum
{
	TBITEM_SELECT = 0,
	TBITEM_EDIT = 1,
	TBITEM_BRUSH = 2,
	TBITEM_ENTITY = 3,
	TBITEM_CONNECTOR = 4
};

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

LRESULT CALLBACK tbProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		

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
				editor->_level.ObjectCollection().Objects().SetSize(0);
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
				editor->_level.ObjectCollection().Objects().SetSize(0);
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

			case TBITEM_SELECT:
				editor->SetTool(Tool::SELECT);
				break;

			case TBITEM_EDIT:
				editor->SetTool(Tool::EDIT);
				break;

			case TBITEM_BRUSH:
				editor->SetTool(Tool::BRUSH);
				break;

			case TBITEM_ENTITY:
				editor->SetTool(Tool::ENTITY);
				break;

			case TBITEM_CONNECTOR:
				editor->SetTool(Tool::CONNECTOR);
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


Editor::Editor() : _propertyWindow(this), _toolWindow(this), _materialManager(_textureManager)
{
}


Editor::~Editor()
{
}

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

	_cameras[0].SetProectionType(ProjectionType::PERSPECTIVE);
	_cameras[0].transform.SetPosition(Vector3(-5.f, 5.f, -5.f));
	_cameras[0].transform.SetRotation(Vector3(-45.f, 45.f, 0.f));

	_cameras[1].SetProectionType(ProjectionType::ORTHOGRAPHIC);
	_cameras[1].SetZBounds(-10000.f, 10000.f);
	_cameras[1].transform.SetRotation(Vector3(-90.f, 0.f, 0.f));

	_cameras[2].SetProectionType(ProjectionType::ORTHOGRAPHIC);
	_cameras[2].SetZBounds(-10000.f, 10000.f);
	_cameras[2].transform.SetRotation(Vector3(0.f, 0.f, 0.f));
	
	_cameras[3].SetProectionType(ProjectionType::ORTHOGRAPHIC);
	_cameras[3].SetZBounds(-10000.f, 10000.f);
	_cameras[3].transform.SetRotation(Vector3(0.f, -90.f, 0.f));


	_inputManager.BindKeyAxis(Keycode::W, &_axisMoveY, 1.f);
	_inputManager.BindKeyAxis(Keycode::S, &_axisMoveY, -1.f);
	_inputManager.BindKeyAxis(Keycode::D, &_axisMoveX, 1.f);
	_inputManager.BindKeyAxis(Keycode::A, &_axisMoveX, -1.f);

	_inputManager.BindKeyAxis(Keycode::UP, &_axisLookY, 1.f);
	_inputManager.BindKeyAxis(Keycode::DOWN, &_axisLookY, -1.f);
	_inputManager.BindKeyAxis(Keycode::RIGHT, &_axisLookX, 1.f);
	_inputManager.BindKeyAxis(Keycode::LEFT, &_axisLookX, -1.f);

	_inputManager.BindKey(Keycode::ENTER, *this, &Editor::Submit);

	_modelManager.Initialise();
	_modelManager.SetRootPath("Data/Models/");

	_textureManager.Initialise();
	_textureManager.SetRootPath("Data/Textures/");

	_materialManager.SetRootPath("Data/Materials/");

	//oof
	Engine::materialManager = &_materialManager;
	Engine::modelManager = &_modelManager;

	_registry.RegisterEngineObjects();


	//Tool data init

	_toolData.brush2D.object.SetMaterial("bricks");
	_toolData.brush2D.properties.SetBase(_toolData.brush2D.object);
	_toolData.brush2D.properties.Add<Brush2D, String>("Material", &_toolData.brush2D.object, &Brush2D::GetMaterialName, &Brush2D::SetMaterial, PropertyFlags::MATERIAL);
	_toolData.brush2D.properties.Add<float>("Level", _toolData.brush2D.object.level);

	_toolData.brush3D.object.SetMaterial("alt");
	_toolData.brush3D.properties.SetBase(_toolData.brush3D.object);
	_toolData.brush3D.properties.Add<Brush3D, String>("Material", &_toolData.brush3D.object, &Brush3D::GetMaterialName, &Brush3D::SetMaterial, PropertyFlags::MATERIAL);
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
	
	//hm
	SetTool(Tool::EDIT);

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

	_cameras[0].transform.Move(
		_cameras[0].transform.GetForwardVector() * _deltaTime * _axisMoveY * moveSpeed 
		+ _cameras[0].transform.GetRightVector() * _deltaTime * _axisMoveX * moveSpeed);

	_cameras[0].transform.Rotate(Vector3(_deltaTime * _axisLookY * rotSpeed, _deltaTime * _axisLookX * rotSpeed, 0.f));

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
	bool persp = _cameras[index].GetProjectionType() == ProjectionType::PERSPECTIVE;
	float boundsScale = persp ? 10.f : 1.f;

	_glContext.Use(_viewports[index]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_shaderUnlit.Use();
	_shaderUnlit.SetMat4(DefaultUniformVars::mat4Projection, _cameras[index].GetProjectionMatrix());
	_shaderUnlit.SetMat4(DefaultUniformVars::mat4View, _cameras[index].MakeInverseTransformationMatrix());
	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 1.f, 1.f, 1.f));
	_level.ObjectCollection().Render();

	if (_tool == Tool::EDIT)
	{
		if (persp) _shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(.8f, .8f, .8f, .5f));
		_toolData.brush2D.object.Render();
	}
	else if (_tool == Tool::BRUSH)
	{
		if (persp) _shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(.8f, .8f, .8f, .5f));
		_toolData.brush3D.object.Render();
	}

	if (!persp) glDepthFunc(GL_ALWAYS);
	
	_textureManager.White().Bind(0);
	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(.75f, .75f, .75f, 1.f));
	DrawUtils::DrawGrid(_modelManager, _cameras[index], dir, 1.f, 1.f, boundsScale);

	if (persp) glDepthFunc(GL_LEQUAL);
	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(.5f, .5f, 1.f, 1.f));
	DrawUtils::DrawGrid(_modelManager, _cameras[index], dir, 1.f, 10.f, boundsScale);
	if (persp) glDepthFunc(GL_LESS);

	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(.8f, .2f, 0.f, 1.f));
	DrawUtils::DrawLine(_modelManager, rcp1, rcp2);

	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(0.f, 1.f, 0.f, 1.f));
	if (_tool == Tool::CONNECTOR)
	{
		glLineWidth(lineW_ConnectorSelected);
		_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(0.f, 1.f, 0.f, 1.f));
		_toolData.connector.object.Render();
	}

	glLineWidth(lineW_Connector);
	for (uint32 i = 0; i < _level.Connectors().GetSize(); ++i)
		_level.Connectors()[i].Render();

	glLineWidth(lineW);

	glDepthFunc(GL_LESS);

	_viewports[index].SwapBuffers();
}

void Editor::UpdateMousePosition(int vpIndex, unsigned short x, unsigned short y)
{
	Camera& camera = _cameras[vpIndex];
	Vector3 right = camera.transform.GetRightVector();
	Vector3 up = camera.transform.GetUpVector();
	int rightAxis = right[0] ? 0 : right[1] ? 1 : 2;
	int upAxis = up[0] ? 0 : up[1] ? 1 : 2;
	int otherAxis = (rightAxis != 0 && upAxis != 0) ? 0 : (rightAxis != 1 && upAxis != 1) ? 1 : 2;

	_mouseData.viewport = vpIndex;
	_mouseData.x = x - (camera.GetViewport()[0] / 2);
	_mouseData.y = -(y - (camera.GetViewport()[1] / 2));
	_mouseData.unitX = camera.transform.Position()[rightAxis] + (float)_mouseData.x / camera.GetScale();
	_mouseData.unitY = camera.transform.Position()[upAxis] + (float)_mouseData.y / camera.GetScale();
	_mouseData.unitX_rounded = _mouseData.unitX < 0.f ? (int)(_mouseData.unitX - 1.f) : (int)_mouseData.unitX;
	_mouseData.unitY_rounded = _mouseData.unitY < 0.f ? (int)(_mouseData.unitY - 1.f) : (int)_mouseData.unitY;

	if (camera.GetProjectionType() == ProjectionType::ORTHOGRAPHIC)
	{
		_window.SetTitle(CSTR(String::Convert(_mouseData.viewport) + " Mouse X:" + String::Convert(_mouseData.x) + " (" + String::ConvertFloat(_mouseData.unitX, 0, 2) + " ) Mouse Y:" + String::Convert(_mouseData.y) + " (" + String::ConvertFloat(_mouseData.unitY, 0, 2) + ')'));
	
		if (!_mouseData.isLeftDown)
		{
			switch (_tool)
			{
			case Tool::EDIT:
				_toolData.brush2D.object.SetPoint1(Vector2((float)_mouseData.unitX_rounded, (float)_mouseData.unitY_rounded));
				_toolData.brush2D.object.SetPoint2(Vector2((float)(_mouseData.unitX_rounded + 1), (float)(_mouseData.unitY_rounded + 1)));
				break;

			case Tool::BRUSH:
				if (!_toolData.placing)
				{
					Vector3 p1;
					p1[rightAxis] = (float)_mouseData.unitX_rounded;
					p1[upAxis] = (float)_mouseData.unitY_rounded;
					p1[otherAxis] = -100;

					Vector3 p2;
					p2[rightAxis] = (float)(_mouseData.unitX_rounded + 1);
					p2[upAxis] = (float)(_mouseData.unitY_rounded + 1);
					p2[otherAxis] = 100;

					_toolData.brush3D.object.SetPoint1(p1);
					_toolData.brush3D.object.SetPoint2(p2);
				}
				break;

			case Tool::CONNECTOR:
				if (!_toolData.placing)
				{
					_toolData.connector.object.point1[rightAxis] = _mouseData.unitX_rounded;
					_toolData.connector.object.point1[upAxis] = _mouseData.unitY_rounded;
					_toolData.connector.object.point1[otherAxis] = -100;
					_toolData.connector.object.point2[rightAxis] = _mouseData.unitX_rounded + 1;
					_toolData.connector.object.point2[upAxis] = _mouseData.unitY_rounded + 1;
					_toolData.connector.object.point2[otherAxis] = 100;
				}
				break;
			}
		}
		else
		{
			int p1x;
			int p1y;
			int p2x;
			int p2y;

			if (_mouseData.unitX_rounded <= _mouseData.heldUnitX_rounded)
			{
				p1x = _mouseData.heldUnitX_rounded + 1;
				p2x = _mouseData.unitX_rounded;
			}
			else
			{
				p1x = _mouseData.heldUnitX_rounded;
				p2x = _mouseData.unitX_rounded + 1;
			}

			if (_mouseData.unitY_rounded <= _mouseData.heldUnitY_rounded)
			{
				p1y = _mouseData.heldUnitY_rounded + 1;
				p2y = _mouseData.unitY_rounded;
			}
			else
			{
				p1y = _mouseData.heldUnitY_rounded;
				p2y = _mouseData.unitY_rounded + 1;
			}

			switch (_tool)
			{
			case Tool::EDIT:
				_toolData.brush2D.object.SetPoint1(Vector2((float)p1x, (float)p1y));
				_toolData.brush2D.object.SetPoint2(Vector2(((float)p2x), (float)(p2y)));
				break;

			case Tool::BRUSH:
			{
				Vector3 p1 = _toolData.brush3D.object.GetPoint1();
				Vector3 p2 = _toolData.brush3D.object.GetPoint2();

				p1[rightAxis] = (float)p1x;
				p1[upAxis] = (float)p1y;
				p2[rightAxis] = (float)p2x;
				p2[upAxis] = (float)p2y;

				_toolData.brush3D.object.SetPoint1(p1);
				_toolData.brush3D.object.SetPoint2(p2);
			}
				break;

			case Tool::CONNECTOR:
				_toolData.connector.object.point1[rightAxis] = p1x;
				_toolData.connector.object.point1[upAxis] = p1y;
				_toolData.connector.object.point2[rightAxis] = p2x;
				_toolData.connector.object.point2[upAxis] = p2y;
				break;
			}
		}
	}
	else
		_window.SetTitle(CSTR(String::Convert(_mouseData.viewport) + " Mouse X:" + String::Convert(_mouseData.x) + " Mouse Y:" + String::Convert(_mouseData.y)));

	_propertyWindow.Refresh();
}

void Editor::LeftMouseDown()
{
	::SetFocus(_window.GetHwnd());

	_mouseData.isLeftDown = true;
	_mouseData.heldUnitX_rounded = _mouseData.unitX_rounded;
	_mouseData.heldUnitY_rounded = _mouseData.unitY_rounded;

	if (_cameras[_mouseData.viewport].GetProjectionType() == ProjectionType::PERSPECTIVE && _tool == Tool::SELECT)
	{
		Camera &camera = _cameras[_mouseData.viewport];

		RECT windowDims;
		::GetClientRect(_viewports[0].GetHwnd(), &windowDims);

		Vector2 scale = camera.GetZPlaneDimensions();

		float mx = (float)_mouseData.x / (float)windowDims.right * scale[0];
		float my = (float)_mouseData.y / (float)windowDims.bottom * scale[1];

		Vector3 pointOnPlane = VectorMaths::Rotate(Vector3(mx, my, 1.f), camera.transform.Rotation());
		pointOnPlane.Normalise();
		Ray r(camera.transform.Position(), pointOnPlane);

		rcp1 = r.origin;
		rcp2 = r.origin + r.direction * 20.f;

		Buffer<RaycastResult> results = _level.ObjectCollection().Raycast(r);

		if (results.GetSize() > 0)
			_propertyWindow.SetObject(results[0].object);
		else
			_propertyWindow.Clear();
	}
	else _toolData.placing = true;
}

void Editor::LeftMouseUp()
{
	_mouseData.isLeftDown = false;

	if (_cameras[_mouseData.viewport].GetProjectionType() == ProjectionType::ORTHOGRAPHIC) {
	switch (_tool)
	{
	case Tool::EDIT:
		if (_mouseData.viewport != 0)
		{
			*_level.ObjectCollection().NewObject<Brush2D>() = _toolData.brush2D.object;
		}
		break;
	}
	}
}

void Editor::Zoom(float amount)
{
	Camera& camera = _cameras[_mouseData.viewport];

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

void Editor::Submit()
{
	if (_tool == Tool::CONNECTOR)
	{
		_toolData.placing = false;

		_level.Connectors().Add(_toolData.connector.object);
		_toolData.connector.object.point1 = Vector<int16, 3>(0, 0, 0);
		_toolData.connector.object.point2 = Vector<int16, 3>(0, 0, 0);
	}
	else if (_tool == Tool::BRUSH)
	{
		_toolData.placing = false;
		
		*_level.ObjectCollection().NewObject<Brush3D>() = _toolData.brush3D.object;
	}
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

	_cameras[0].SetViewport(vpW, vpH);
	_cameras[1].SetViewport(vpW, vpH);
	_cameras[2].SetViewport(vpW, vpH);
	_cameras[3].SetViewport(vpW, vpH);

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

void Editor::SetTool(Tool newTool)
{
	_propertyWindow.Clear();

	switch (newTool)
	{

	case Tool::EDIT:
		_propertyWindow.SetObject(&_toolData.brush2D.object, true);
		_toolWindow.PropertyWindow().SetProperties(_toolData.brush2D.properties);
		break;
		
	case Tool::BRUSH:
		_propertyWindow.SetObject(&_toolData.brush3D.object, true);
		_toolWindow.PropertyWindow().SetProperties(_toolData.brush3D.properties);
		break;

	case Tool::CONNECTOR:
		_propertyWindow.Clear();
		_toolWindow.PropertyWindow().Clear();
		break;

	default:
		_propertyWindow.Clear();
		_toolWindow.PropertyWindow().Clear();
	}

	_tool = newTool;
}
