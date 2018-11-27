#include "Game.h"
#include <Engine/Engine.h>
#include <Engine/GL.h>
#include <windowsx.h>

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

	case WM_SIZE:
		game->Resize(LOWORD(lparam), HIWORD(lparam));
		game->Render();
		break;

	case WM_MOUSEMOVE:
		game->MouseMove(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;

	case WM_LBUTTONDOWN:
		game->MouseDown();
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

void Game::_InitWindow()
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
}

void Game::_InitGL()
{
	_glContext.Create(_window);
	_glContext.Use(_window);
	GL::LoadExtensions(_window.GetHDC());

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.f, 0.f, 1.f);
	wglSwapIntervalEXT(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	_shader.Load("Data/Shaders/Shader.vert", "Data/Shaders/Unlit.frag");
}

void Game::_Init()
{
	_fontManager.SetRootPath("Data/Fonts/");

	_modelManager.Initialise();
	_modelManager.SetRootPath("Data/Models/");

	_textureManager.Initialise();
	_textureManager.SetRootPath("Data/Textures/");

	_materialManager.SetRootPath("Data/Materials/");

	Engine::fontManager = &_fontManager;
	Engine::inputManager = &_inputManager;
	Engine::materialManager = &_materialManager;
	Engine::modelManager = &_modelManager;
	Engine::textureManager = &_textureManager;


	_mainMenu.Initialise();
	_mainMenu.SetOnStart(Callback(this, &Game::ButtonStart));
	_mainMenu.SetOnQuit(Callback(this, &Game::ButtonQuit));

	_uiCamera.SetProectionType(ProjectionType::ORTHOGRAPHIC);
	_uiCamera.SetScale(1.f);
	_uiCamera.SetZBounds(-10.f, 10.f);
}

void Game::Run()
{
	_InitWindow();
	_InitGL();
	_Init();

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

	glDepthFunc(GL_LEQUAL);

	_shader.Use();
	_shader.SetMat4(DefaultUniformVars::mat4Projection, _uiCamera.GetProjectionMatrix());
	_shader.SetMat4(DefaultUniformVars::mat4View, _uiCamera.transform.GetInverseTransformationMatrix());

	_mainMenu.Render();

	_window.SwapBuffers();
}

void Game::Resize(uint16 w, uint16 h)
{
	_mainMenu.SetBounds(w / 2 - 200, h / 2 - 200, 400, 400);
	_uiCamera.SetViewport(w, h);
	_uiCamera.transform.SetPosition(Vector3(w / 2.f, h / 2.f, 0.f));
	glViewport(0, 0, w, h);
}

void Game::MouseMove(unsigned short x, unsigned short y)
{
	_mainMenu.OnMouseMove((float)x, _uiCamera.GetViewport()[1] - y);
}

void Game::MouseDown()
{
	_mainMenu.OnClick();
}

void Game::ButtonStart()
{
	::MessageBoxA(NULL, "Ha, you wish!", "Maybe Later", MB_OK);
}

void Game::ButtonQuit()
{
	_running = false;
}
