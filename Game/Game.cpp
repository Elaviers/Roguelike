#include "Game.h"
#include <Engine/AudioManager.h>
#include <Engine/Console.h>
#include <Engine/Engine.h>
#include <Engine/FontManager.h>
#include <Engine/GL.h>
#include <Engine/IO.h>
#include <windowsx.h>
#include "LevelGeneration.h"
#include "MenuMain.h"

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

	case WM_KEYDOWN:
		if (lparam& (1 << 30))
			break; //Key repeats ignored

		game->KeyDown((Keycode)wparam);
		break;

	case WM_CHAR:
		game->InputChar((char)wparam);
		break;

	default: return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

void Game::_InitWindow()
{
	{
		LPCTSTR dummyClassName = TEXT("DUMMY");
		LPCTSTR className = TEXT("GAMEWINDOW");

		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.lpfnWndProc = ::DefWindowProc;
		windowClass.hInstance = ::GetModuleHandle(NULL);
		windowClass.lpszClassName = dummyClassName;
		::RegisterClassEx(&windowClass);

		windowClass.lpfnWndProc = _WindowProc;
		windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		windowClass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
		windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = className;
		::RegisterClassEx(&windowClass);

		GLContext dummy = GLContext::CreateDummyAndUse(dummyClassName);
		GL::LoadDummyExtensions();

		_window.Create(className, "Window", this);
			
		dummy.Delete();
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

	_shader.Load("Data/Shaders/Shader.vert", "Data/Shaders/Unlit.frag", ShaderChannel::ALL);
}

void Game::_Init()
{
	Engine::Instance().InitFT();
	Engine::Instance().DefaultInit();

	String fontDir;
	fontDir.SetLength(MAX_PATH);

	::GetWindowsDirectoryA(&fontDir[0], fontDir.GetLength());

	fontDir.Trim();
	fontDir += "\\FONTS\\";

	Engine::Instance().pFontManager->AddPath(fontDir);

	_consoleIsActive = false;

	_uiCamera.SetProectionType(ProjectionType::ORTHOGRAPHIC);
	_uiCamera.SetScale(1.f);
	_uiCamera.SetZBounds(-10.f, 10.f);
}

void Game::Run()
{
	_InitWindow();
	_InitGL();
	_Init();

	MenuMain *menu = new MenuMain();
	menu->SetBounds(0.5f, 0.5f, 400, 400, -200, -200);
	menu->Initialise(FunctionPointer<void, const String&>(this, &Game::StartLevel), Callback(this, &Game::ButtonQuit));
	menu->SetParent(&_ui);

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

void Game::StartLevel(const String &level)
{
	_running = false;
	
	String fileString = IO::ReadFileString(level.GetData());
	GameObject world = LevelGeneration::GenerateLevel(fileString);

}

void Game::Frame()
{
	_timer.Start();

	Engine::Instance().pAudioManager->FillBuffer();

	_ui.Update();

	Render();
	_deltaTime = _timer.SecondsSinceStart();
}

void Game::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthFunc(GL_LEQUAL);

	_shader.Use();
	_shader.SetMat4(DefaultUniformVars::mat4Projection, _uiCamera.GetProjectionMatrix());
	_shader.SetMat4(DefaultUniformVars::mat4View, _uiCamera.GetInverseTransformationMatrix());

	_ui.Render();

	if (_consoleIsActive)
	{
		_shader.SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 1.f, 1.f, 1.f));
		Engine::Instance().pConsole->Render(**Engine::Instance().pFontManager->Get("consolas"), _deltaTime);
	}

	_window.SwapBuffers();
}

void Game::Resize(uint16 w, uint16 h)
{
	_ui.SetBounds(0, 0, w, h);
	_uiCamera.SetViewport(w, h);
	_uiCamera.SetRelativePosition(Vector3(w / 2.f, h / 2.f, 0.f));
	glViewport(0, 0, w, h);
}

void Game::MouseMove(unsigned short x, unsigned short y)
{
	_ui.OnMouseMove((float)x, (float)(_uiCamera.GetViewport()[1] - y));
}

void Game::MouseDown()
{
	_ui.OnClick();
}

void Game::KeyDown(Keycode key)
{
	if (key == Keycode::TILDE)
	{
		_consoleIsActive = !_consoleIsActive;
		return;
	}
	
	if (_consoleIsActive)
		Engine::Instance().pInputManager->KeyDown(key);
}

void Game::InputChar(char character)
{
	if (_consoleIsActive)
	{
		if (character == '`')
			return;

		Engine::Instance().pConsole->InputChar(character);
	}

}

void Game::ButtonQuit()
{
	_running = false;
}
