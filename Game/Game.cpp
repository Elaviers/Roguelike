#include "Game.hpp"
#include <Engine/AudioManager.hpp>
#include <Engine/Console.hpp>
#include <Engine/Engine.hpp>
#include <Engine/EntLight.hpp>
#include <Engine/FontManager.hpp>
#include <Engine/GL.hpp>
#include <Engine/LevelIO.hpp>
#include <Engine/IO.hpp>
#include <windowsx.h>
#include "EntPlayer.h"
#include "GameInstance.h"
#include "LevelGeneration.hpp"
#include "MenuMain.hpp"

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

	case WM_LBUTTONUP:
		game->MouseUp();
		break;

	case WM_KEYDOWN:
		if (lparam& (1 << 30))
			break; //Key repeats ignored

		game->KeyDown((EKeycode)wparam);
		break;

	case WM_KEYUP:
		game->KeyUp((EKeycode)wparam);

		break;

	case WM_CHAR:
		game->InputChar((char)wparam);
		break;

	case WM_INPUT:
	{
		static Buffer<byte> buffer;

		UINT size;
		GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

		if (buffer.GetSize() < size)
			buffer.SetSize(size);

		GetRawInputData((HRAWINPUT)lparam, RID_INPUT, &buffer[0], &size, sizeof(RAWINPUTHEADER));

		RAWINPUT* input = (RAWINPUT*)buffer.Data();
		if (input->header.dwType == RIM_TYPEMOUSE)
		{
			game->MouseInput((short)input->data.mouse.lLastX, (short)input->data.mouse.lLastY);
		}
	}
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

		GLContext dummy;
		dummy.CreateDummyAndUse(dummyClassName);
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

	glEnable(GL_MULTISAMPLE);

	_shaderLit.SetMaxLightCount(8);
	_shaderLit.Load("Data/Shaders/Shader.vert", "Data/Shaders/Phong.frag");
	_shaderUnlit.Load("Data/Shaders/Shader.vert", "Data/Shaders/Unlit.frag");
}

void Game::_Init()
{
	RAWINPUTDEVICE rawMouseInput;
	rawMouseInput.usUsagePage = 0x01;
	rawMouseInput.usUsage = 0x02;
	rawMouseInput.dwFlags = 0;
	rawMouseInput.hwndTarget = 0;

	if (RegisterRawInputDevices(&rawMouseInput, 1, sizeof(rawMouseInput)) == FALSE)
	{
		Debug::Error("Raw mouse input registration error!");
	}

	Engine::Instance().Init(EEngineCreateFlags::ALL, nullptr);
	Engine::Instance().pFontManager->AddPath(Utilities::GetSystemFontDir());

	Engine::Instance().pWorld = &_world;

	GameInstance::Instance().SetupInputs();

	_consoleIsActive = false;
	_uiIsActive = true;

	_uiCamera.SetProjectionType(EProjectionType::ORTHOGRAPHIC);
	_uiCamera.SetScale(1.f);
	_uiCamera.SetZBounds(-10.f, 10.f);
}

void Game::Run()
{
	_InitWindow();
	_InitGL();
	_Init();

	MenuMain *menu = new MenuMain();
	menu->SetBounds(0.5f, 0.5f, 512, 512, -256, -256);
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

void Game::StartLevel(const String& filename)
{
	String ext = Utilities::GetExtension(filename);

	if (ext == ".txt")
	{
		_world.DeleteChildren();

		String fileString = IO::ReadFileString(filename.GetData());
		LevelGeneration::GenerateLevel(_world, fileString);
	}
	else
	{
		_world.DeleteChildren();
		LevelIO::Read(_world, filename.GetData());
	}

	EntPlayer* player = EntPlayer::Create();
	player->SetParent(&_world);

	EntLight* light = EntLight::Create();
	light->SetParent(player);

	Transform spawnTransform(Vector3(0, 4.f, 0));
	player->SetRelativeTransform(spawnTransform);

	_uiIsActive = false;
}

void Game::Frame()
{
	_timer.Start();

	_window.SetTitle(CSTR(_mouseXForFrame, " ", _mouseYForFrame));

	Engine::Instance().pInputManager->SetMouseAxes(_mouseXForFrame, _mouseYForFrame);
	_mouseXForFrame = _mouseYForFrame = 0;

	Engine::Instance().pAudioManager->FillBuffer();

	_world.UpdateAll(_deltaTime);
	_ui.Update(_deltaTime);

	Render();
	_deltaTime = _timer.SecondsSinceStart();
}

void Game::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthFunc(GL_LEQUAL);


	const EntCamera* activeCamera = GameInstance::Instance().GetActiveCamera();
	if (activeCamera)
	{
		_shaderLit.Use();
		_shaderLit.SetVec2(DefaultUniformVars::vec2UVOffset, Vector2());
		_shaderLit.SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1, 1));
		_shaderLit.SetVec4(DefaultUniformVars::vec4Colour, Colour::White);
		_shaderLit.SetInt(DefaultUniformVars::intTextureDiffuse, 0);
		_shaderLit.SetInt(DefaultUniformVars::intTextureNormal, 1);
		_shaderLit.SetInt(DefaultUniformVars::intTextureSpecular, 2);
		_shaderLit.SetInt(DefaultUniformVars::intTextureReflection, 3);

		activeCamera->Use();
		_world.RenderAll(*activeCamera, ERenderChannels::PRE_RENDER);
		EntLight::FinaliseLightingForFrame();
		_world.RenderAll(*activeCamera, ERenderChannels::SURFACE);
	}

	_shaderUnlit.Use();
	_uiCamera.Use();

	if (_uiIsActive)
		_ui.Render();

	if (_consoleIsActive)
	{
		_shaderUnlit.SetVec4(DefaultUniformVars::vec4Colour, Colour::White);
		Engine::Instance().pConsole->Render(*Engine::Instance().pFontManager->Get("consolas"), _deltaTime);
	}

	_window.SwapBuffers();
}

void Game::Resize(uint16 w, uint16 h)
{
	GameInstance::Instance().OnResize(w, h);

	_ui.SetBounds(0, 0, w, h);
	_uiCamera.SetViewport(w, h);
	_uiCamera.SetRelativePosition(Vector3(w / 2.f, h / 2.f, 0.f));
}

void Game::MouseInput(short x, short y)
{
	_mouseXForFrame += x;
	_mouseYForFrame += y;
}

void Game::MouseMove(uint16 x, uint16 y)
{
	if (_uiIsActive)
	{
		_ui.OnMouseMove((float)x, (float)(_uiCamera.GetViewport()[1] - y));
	}
}

void Game::MouseUp()
{
	if (_uiIsActive)
	{
		_ui.OnMouseUp();
	}
}

void Game::MouseDown()
{
	if (_uiIsActive)
	{
		_ui.OnMouseDown();
	}
}

void Game::KeyDown(EKeycode key)
{
	if (_uiIsActive)
		_ui.OnKeyDown(key);

	Engine::Instance().pInputManager->KeyDown(key);

	if (key == EKeycode::TILDE)
	{
		_consoleIsActive = !_consoleIsActive;
		return;
	}
}

void Game::KeyUp(EKeycode key)
{
	if (_uiIsActive)
		_ui.OnKeyUp(key);
	
	Engine::Instance().pInputManager->KeyUp(key);
}

void Game::InputChar(char character)
{
	if (_uiIsActive)
		_ui.OnCharInput(character);

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
