#include "Editor.h"
#include <Engine/DrawUtils.h>
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

		editor->_viewports[0].UseGLContext();
		glViewport(0, 0, w, h);
		editor->_viewports[1].UseGLContext();
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

	_InitGL();

	_modelManager.Initialise();
	_textureManager.Initialise();

	_cameras[0].SetProectionType(ProjectionType::PERSPECTIVE);
	_cameras[1].SetProectionType(ProjectionType::ORTHOGRAPHIC);
	_cameras[1].transform.SetPosition(Vector3(0.f, 10.f, 0.f));
	_cameras[1].transform.SetRotation(Vector3(-90.f, 0.f, 0.f));

	_viewports[0].UseGLContext();
	glClearColor(1.f, 0.f, 0.f, 1.f);
}

void Editor::_InitGL()
{
	for (unsigned int i = 0; i < 2; ++i)
	{
		_viewports[i].UseGLContext();
		GL::LoadExtensions(_viewports[i].GetHDC());
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		wglSwapIntervalEXT(0);
	}

	_shader.Load("Data/Shaders/Shader.vert", "Data/Shaders/Shader.frag");
	_basicShader.Load("Data/Shaders/Basic.vert", "Data/Shaders/Basic.frag");
}

void Editor::Run()
{
	_Init();

	_window.Show();

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
	_timer.Start();

	_window.SetTitle(CSTR(String::Convert(_mouseViewport) + " Mouse X:" + String::Convert(_mouseX) + " Mouse Y:" + String::Convert(_mouseY)));

	Render();

	_deltaTime = _timer.SecondsSinceStart();
}

void Editor::Render()
{
	//View 0
	_viewports[0].UseGLContext();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	


	_viewports[0].SwapBuffers();

	//View 1
	_viewports[1].UseGLContext();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const uint16 vpw = _cameras[1].GetViewport()[0] / 2;
	const int gap = 32;

	glDepthFunc(GL_ALWAYS);

	_basicShader.Use();
	_basicShader.SetMat4("M_Projection", _cameras[1].GetProjectionMatrix());
	_basicShader.SetMat4("M_View", _cameras[1].MakeInverseTransformationMatrix());
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
