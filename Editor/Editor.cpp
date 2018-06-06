#include "Editor.h"

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
		uint16 h = HIWORD(lparam) / 2;

		editor->_viewports[0].SetSizeAndPos(0, 0, w, h);
		editor->_viewports[1].SetSizeAndPos(w, 0, w, h);
		editor->_viewports[2].SetSizeAndPos(0, h, w, h);
		editor->_viewports[3].SetSizeAndPos(w, h, w, h);

		editor->Render();
		break;
	}

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
		::RegisterClassEx(&windowClass);

		_window.Create(className, "Window", this);
	}

	_textureManager.Initialise();

	Viewport::Initialise();
	_viewports[0].Create(_window.GetHwnd());
	_viewports[1].Create(_window.GetHwnd());
	_viewports[2].Create(_window.GetHwnd());
	_viewports[3].Create(_window.GetHwnd());
}

void Editor::_InitGL()
{
	for (unsigned int i = 0; i < 4; ++i)
	{
		_viewports[0].UseGLContext();
		GL::LoadExtensions(_viewports[i].GetHDC());
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		//glClearColor(0.2f, 0.f, 0.f, 1.f);
		wglSwapIntervalEXT(0);
	}

	//_shader.Load("Data/Shader.vert", "Data/Shader.frag");
}

void Editor::Run()
{
	_Init();
	_InitGL();

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

	Render();

	_deltaTime = _timer.SecondsSinceStart();
}

void Editor::Render()
{
	//View 0
	_viewports[0].UseGLContext();
	glClearColor(1.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	_viewports[0].SwapBuffers();

	//View 1
	_viewports[1].UseGLContext();
	glClearColor(0.f, 1.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	_viewports[1].SwapBuffers();
	//View 2
	_viewports[2].UseGLContext();
	glClearColor(0.f, 0.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	_viewports[2].SwapBuffers();
	//View 3
	_viewports[3].UseGLContext();
	glClearColor(1.f, 1.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	_viewports[3].SwapBuffers();
}
