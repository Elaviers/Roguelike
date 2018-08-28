#include "Editor.h"
#include <Engine/DrawUtils.h>
#include <Engine/IO.h>
#include <Engine/Renderable.h>
#include "resource.h"
#include "ResourceSelect.h"

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
		break;
	}

	return ::DefWindowProc(hwnd, msg, wparam, lparam);
}

LRESULT CALLBACK Editor::_WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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

	case WM_CLOSE:
		::DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		editor->_running = false;
		break;

	case WM_SIZE:
	{
		editor->Resize(LOWORD(lparam), HIWORD(lparam));
		break;
	}

	case WM_COMMAND:
		switch (HIWORD(wparam))
		{
		case 0:
			switch (LOWORD(wparam))
			{
			case ID_VIEW_PROPERTIES:
				editor->_propertyWindow.Show();
				break;

			case ID_VIEW_MATERIALS:
				editor->SelectMaterialDialog();
				break;

			case ID_VIEW_MODELS:
				editor->SelectModelDialog();
			break;

			case ID_HELP_ABOUT:
				::DialogBox(NULL, MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutProc);
				break;
			}
		}

		break;

	case WM_MOUSEWHEEL:
		if ((signed short)HIWORD(wparam) > 0)
			editor->Zoom(1.25f);
		else
			editor->Zoom(.75f);
		break;

	case WM_KEYDOWN:
		if (lparam & (1 << 30))
			break; //Key repeats ignored

		editor->_inputManager.KeyDown((Keycode)wparam);
		break;
	case WM_KEYUP:

		editor->_inputManager.KeyUp((Keycode)wparam);
		break;

	default: return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}


Editor::Editor() : _propertyWindow(this)
{
}


Editor::~Editor()
{
}

void Editor::_Init()
{
	{
		LPCTSTR className = TEXT("MAINWINDOW");

		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.lpfnWndProc = _WindowProc;
		windowClass.hInstance = ::GetModuleHandle(NULL);
		windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		windowClass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
		windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = className;
		windowClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
		::RegisterClassEx(&windowClass);

		_window.Create(className, "Window", this);
	}

	ResourceSelect::Initialise();

	PropertyWindow::Initialise();
	_propertyWindow.Create();

	Viewport::Initialise();
	_viewports[0].Create(_window.GetHwnd(), *this, 0);
	_viewports[1].Create(_window.GetHwnd(), *this, 1);

	_InitGL();

	_cameras[0].SetProectionType(ProjectionType::PERSPECTIVE);
	_cameras[0].transform.SetPosition(Vector3(5.f, 5.f, 5.f));
	_cameras[0].transform.SetRotation(Vector3(-45.f, -135.f, 0.f));

	_cameras[1].SetProectionType(ProjectionType::ORTHOGRAPHIC);
	_cameras[1].SetZBounds(-10000.f, 10000.f);
	_cameras[1].transform.SetRotation(Vector3(-90.f, 0.f, 0.f));

	_inputManager.BindKeyAxis(Keycode::W, &_axisMoveY, 1.f);
	_inputManager.BindKeyAxis(Keycode::S, &_axisMoveY, -1.f);
	_inputManager.BindKeyAxis(Keycode::D, &_axisMoveX, 1.f);
	_inputManager.BindKeyAxis(Keycode::A, &_axisMoveX, -1.f);

	_inputManager.BindKeyAxis(Keycode::UP, &_axisLookY, 1.f);
	_inputManager.BindKeyAxis(Keycode::DOWN, &_axisLookY, -1.f);
	_inputManager.BindKeyAxis(Keycode::RIGHT, &_axisLookX, 1.f);
	_inputManager.BindKeyAxis(Keycode::LEFT, &_axisLookX, -1.f);

	_modelManager.Initialise();
	_modelManager.SetRootPath("Data/Models/");

	_textureManager.Initialise();
	_textureManager.SetRootPath("Data/Textures/");

	_materialManager.SetRootPath("Data/Materials/");

	//oof
	Renderable::SetManagers(&_materialManager, &_modelManager);

	_registry.RegisterEngineObjects();
}

void Editor::_InitGL()
{
	_glContext.Create(_viewports[0]);
	_glContext.Use(_viewports[0]);

	GL::LoadExtensions(_window.GetHDC());
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	wglSwapIntervalEXT(0);

	_shaderLit.Load("Data/Shaders/Shader.vert", "Data/Shaders/Phong.frag");
	_shaderUnlit.Load("Data/Shaders/Shader.vert", "Data/Shaders/Unlit.frag");
	_basicShader.Load("Data/Shaders/Basic.vert", "Data/Shaders/Basic.frag");
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
		_materialManager.GetMaterial(_textureManager, filenames[i]);
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

	Renderable *r = new Renderable();
	r->SetModel("sphere");
	_propertyWindow.SetObject(r);

	_currentObject = r;
	_gameObjects.Add(_currentObject);

	for (int i = 0; i < 4; ++i)
		_viewports[i].Show();

	_propertyWindow.Show();

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
}

void Editor::Frame()
{
	const float moveSpeed = 4.f; // units/s
	const float rotSpeed = 90.f; //	degs/s

	_timer.Start();

	_window.SetTitle(CSTR(String::Convert(_mouseViewport) + " Mouse X:" + String::Convert(_mouseX) + " Mouse Y:" + String::Convert(_mouseY)));

	_cameras[0].transform.Move(
		_cameras[0].transform.GetForwardVector() * _deltaTime * _axisMoveY * moveSpeed 
		+ _cameras[0].transform.GetRightVector() * _deltaTime * _axisMoveX * moveSpeed);

	_cameras[0].transform.Rotate(Vector3(_deltaTime * _axisLookY * rotSpeed, _deltaTime * _axisLookX * rotSpeed, 0.f));

	Render();

	_deltaTime = _timer.SecondsSinceStart();
}

void RenderObjects(const Buffer<GameObject*>& objects)
{
	for (uint32 i = 0; i < objects.GetSize(); ++i)
		((Renderable*)objects[i])->Render();
}

void Editor::Render()
{
	if (_gameObjects.GetSize() == 0) return; //remove this

	//View 0
	_glContext.Use(_viewports[0]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_shaderUnlit.Use();
	_shaderUnlit.SetMat4(DefaultUniformVars::mat4Projection, _cameras[0].GetProjectionMatrix());
	_shaderUnlit.SetMat4(DefaultUniformVars::mat4View, _cameras[0].MakeInverseTransformationMatrix());
	_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 1.f, 1.f, 1.f));
	RenderObjects(_gameObjects);

	_basicShader.Use();

	_basicShader.SetMat4(DefaultUniformVars::mat4Projection, _cameras[0].GetProjectionMatrix());
	_basicShader.SetMat4(DefaultUniformVars::mat4View, _cameras[0].MakeInverseTransformationMatrix());

	_basicShader.SetVec4(DefaultUniformVars::vec4Colour, Vector4(.5f, .5f, 1.f, 1.f));
	DrawUtils::DrawGrid(_modelManager, _cameras[0], Direction::DOWN, 2.f, 10.f, 10.f);
	_basicShader.SetVec4(DefaultUniformVars::vec4Colour, Vector4(.75f, .75f, .75f, 1.f));
	DrawUtils::DrawGrid(_modelManager, _cameras[0], Direction::DOWN, 2.f, 1.f, 10.f);

	_viewports[0].SwapBuffers();

	//View 1
	_glContext.Use(_viewports[1]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const uint16 vpw = _cameras[1].GetViewport()[0] / 2;
	const int gap = 32;

	_basicShader.Use();
	_basicShader.SetMat4(DefaultUniformVars::mat4Projection, _cameras[1].GetProjectionMatrix());
	_basicShader.SetMat4(DefaultUniformVars::mat4View, _cameras[1].MakeInverseTransformationMatrix());

	_basicShader.SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 1.f, 1.f, 1.f));
	RenderObjects(_gameObjects);

	glDepthFunc(GL_ALWAYS);
	_basicShader.SetVec4(DefaultUniformVars::vec4Colour, Vector4(.75f, .75f, .75f, 1.f));
	DrawUtils::DrawGrid(_modelManager, _cameras[1], Direction::DOWN, 1.f, 1.f);
	_basicShader.SetVec4(DefaultUniformVars::vec4Colour, Vector4(.5f, .5f, 1.f, 1.f));
	DrawUtils::DrawGrid(_modelManager, _cameras[1], Direction::DOWN, 1.f, 10.f);
	glDepthFunc(GL_LESS);

	_viewports[1].SwapBuffers();
}

void Editor::Zoom(float amount)
{
	//Needs work

	if (_mouseViewport == 1)
	{
		Camera& camera = _cameras[_mouseViewport];

		float vpUnitsX = camera.GetViewport()[0] / camera.GetScale();
		float vpUnitsY = camera.GetViewport()[1] / camera.GetScale();

		float moveX = ((float)_mouseX / (float)camera.GetViewport()[0]) - .5f;
		float moveY = ((float)(camera.GetViewport()[1] - _mouseY) / (float)camera.GetViewport()[1]) - .5f;

		camera.transform.Move(Vector3(vpUnitsX * moveX, vpUnitsY * moveY, 0.f));

		camera.SetScale(_cameras[_mouseViewport].GetScale() * amount);
	}
}

void Editor::Resize(uint16 w, uint16 h)
{
	uint16 vpW = w / 2;

	_viewports[0].SetSizeAndPos(0, 0, vpW, h);
	_cameras[0].SetViewport(vpW, h);
	_viewports[1].SetSizeAndPos(vpW, 0, vpW, h);
	_cameras[1].SetViewport(vpW, h);

	glViewport(0, 0, vpW, h);

	Render();
}

String Editor::SelectMaterialDialog()
{
	String string = ResourceSelect::Dialog(_materialManager, _modelManager, "Data/Materials/*.txt", _propertyWindow.GetHwnd(),
		ResourceType::MATERIAL, _glContext, _shaderLit);

	RECT rect;
	::GetClientRect(_window.GetHwnd(), &rect);
	Resize(rect.right, rect.bottom);

	return string;
}

String Editor::SelectModelDialog()
{
	String string = ResourceSelect::Dialog(_materialManager, _modelManager, "Data/Models/*.txt", _propertyWindow.GetHwnd(),
		ResourceType::MODEL, _glContext, _shaderLit);

	RECT rect;
	::GetClientRect(_window.GetHwnd(), &rect);
	Resize(rect.right, rect.bottom);

	return string;
}

