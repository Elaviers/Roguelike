#include "Game.h"
#include <Engine/GL.h>

LRESULT CALLBACK Game::_WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Game *game = (Game*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);


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
		game->_running = false;
		break;

	default: return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

Game::Game()
{
}


Game::~Game()
{
}

void Game::_Init()
{
	{
		LPCTSTR className = TEXT("GAMEWINDOW");

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
}

void Game::_InitGL()
{
	_window.UseGLContext();
	GL::LoadExtensions(_window.GetHDC());

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.f, 0.f, 1.f);
	wglSwapIntervalEXT(0);

	_shader.Load("Data/Shader.vert", "Data/Shader.frag");
}

void Game::Run()
{
	_Init();
	_InitGL();

	_window.Show();

	MSG msg;
	_running = true;
	while (_running)
	{
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		Frame();
	}
}

void Game::Frame()
{
	_timer.Start();



	Render();
	_deltaTime = _timer.SecondsSinceStart();
}

void Game::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	_window.SwapBuffers();
}
