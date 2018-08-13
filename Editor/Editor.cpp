#include "Editor.h"
#include <Engine/DrawUtils.h>
#include <Engine/Renderable.h>
#include "resource.h"

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
		uint16 w = LOWORD(lparam) / 2;
		uint16 h = HIWORD(lparam);

		editor->_viewports[0].SetSizeAndPos(0, 0, w, h);
		editor->_cameras[0].SetViewport(w, h);
		editor->_viewports[1].SetSizeAndPos(w, 0, w, h);
		editor->_cameras[1].SetViewport(w, h);

		glViewport(0, 0, w, h);

		editor->Render();
		break;
	}

	case WM_COMMAND:
		switch (HIWORD(wparam))
		{
		case 0:
			switch (LOWORD(wparam))
			{
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


Editor::Editor()
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

	Viewport::Initialise();
	_viewports[0].Create(_window.GetHwnd(), *this, 0);
	_viewports[1].Create(_window.GetHwnd(), *this, 1);

	_glContext.Create(_viewports[0]);
	_glContext.Use(_viewports[0]);

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
	_textureManager.Initialise();

	_registry.RegisterEngineObjects();
}

void Editor::_InitGL()
{
	for (unsigned int i = 0; i < 2; ++i)
	{
		GL::LoadExtensions(_viewports[i].GetHDC());
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		wglSwapIntervalEXT(0);
	}

	_shader.Load("Data/Shaders/Shader.vert", "Data/Shaders/Unlit.frag");
	_basicShader.Load("Data/Shaders/Basic.vert", "Data/Shaders/Basic.frag");
}

void Editor::Run()
{
	_Init();
	_window.Show();

	_materialManager.MakeMaterial(_textureManager, "Model", "Data/Textures/Diffuse.png");

	Renderable *r = new Renderable();
	r->SetModel(&_modelManager.GetModel("Data/Models/Model.obj"));
	r->SetMaterial(_materialManager.Get("Model"));

	_currentObject = r;
	_gameObjects.Add(_currentObject);

	for (int i = 0; i < 4; ++i)
		_viewports[i].Show();

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

	_shader.Use();
	_shader.SetMat4("M_Projection", _cameras[0].GetProjectionMatrix());
	_shader.SetMat4("M_View", _cameras[0].MakeInverseTransformationMatrix());
	_shader.SetVec4("Colour", Vector4(1.f, 1.f, 1.f, 1.f));
	RenderObjects(_gameObjects);

	_basicShader.Use();

	_basicShader.SetMat4("M_Projection", _cameras[0].GetProjectionMatrix());
	_basicShader.SetMat4("M_View", _cameras[0].MakeInverseTransformationMatrix());

	_basicShader.SetVec4("Colour", Vector4(.5f, .5f, 1.f, 1.f));
	DrawUtils::DrawGrid(_modelManager, _cameras[0], Direction::DOWN, 2.f, 10.f, 10.f);
	_basicShader.SetVec4("Colour", Vector4(.75f, .75f, .75f, 1.f));
	DrawUtils::DrawGrid(_modelManager, _cameras[0], Direction::DOWN, 2.f, 1.f, 10.f);

	_viewports[0].SwapBuffers();

	//View 1
	_glContext.Use(_viewports[1]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const uint16 vpw = _cameras[1].GetViewport()[0] / 2;
	const int gap = 32;

	_basicShader.Use();
	_basicShader.SetMat4("M_Projection", _cameras[1].GetProjectionMatrix());
	_basicShader.SetMat4("M_View", _cameras[1].MakeInverseTransformationMatrix());

	_basicShader.SetVec4("Colour", Vector4(1.f, 1.f, 1.f, 1.f));
	RenderObjects(_gameObjects);

	glDepthFunc(GL_ALWAYS);
	_basicShader.SetVec4("Colour", Vector4(.75f, .75f, .75f, 1.f));
	DrawUtils::DrawGrid(_modelManager, _cameras[1], Direction::DOWN, 1.f, 1.f);
	_basicShader.SetVec4("Colour", Vector4(.5f, .5f, 1.f, 1.f));
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
