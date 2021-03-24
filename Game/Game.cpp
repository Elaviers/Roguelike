#include "Game.hpp"
#include <ELAudio/AudioManager.hpp>
#include <ELGraphics/FontManager.hpp>
#include <ELMaths/Frustum.hpp>
#include <ELSys/GL.hpp>
#include <ELSys/IO.hpp>
#include <ELSys/Utilities.hpp>
#include <Engine/Console.hpp>
#include <Engine/OLight.hpp>
#include <Engine/GeoIsoTile.hpp>
#include <Engine/imgui/imgui.h>
#include <Engine/imgui/imgui_internal.h>
#include <Engine/imgui/imgui_impl_opengl3.h>
#include <Engine/imgui/imgui_impl_win32.h>
#include <windowsx.h>
#include "OPlayer.hpp"
#include "GameInstance.hpp"
#include "LevelGeneration.hpp"
#include "MenuMain.hpp"

Game::~Game()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

void Game::_InitGL()
{
	_glContext.Create(_window);
	_glContext.Use(_window);
	GL::LoadExtensions(_window);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.f, 0.f, 1.f);
	wglSwapIntervalEXT(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_MULTISAMPLE);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	_shaderLit.Load("Data/Shaders/Shader.vert", "Data/Shaders/Phong.frag");
	_shaderUnlit.Load("Data/Shaders/Shader.vert", "Data/Shaders/Unlit.frag");

	TextureData faces[6];
	for (int i = 0; i < 6; ++i)
	{
		faces[i].data = new byte[4]();
		faces[i].width = 1;
		faces[i].height = 1;
	}

	_reflect.Create(faces);

	for (int i = 0; i < 6; ++i)
		delete faces[i].data;
}

#include <ELSys/TextManager.hpp>

void Game::_Init()
{
	_engine.Init(EEngineCreateFlags::ALL);
	_engine.pFontManager->AddPath(Utilities::GetSystemFontDir());

	_world.Initialise();
	GameInstance::Instance().world = &_world;
	GameInstance::Instance().Initialise(*this);

	_consoleIsActive = false;
	_uiIsActive = true;

	_uiCamera.SetType(EProjectionType::ORTHOGRAPHIC);
	_uiCamera.SetNearFar(-10.f, 10.f);

	//Third party (IMGUI)
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui::GetIO().IniFilename = NULL;

	ImGui_ImplWin32_Init(_window.GetHWND());
	ImGui_ImplOpenGL3_Init("#version 410");
}

void Game::Run()
{
	GLContext dummy;
	dummy.CreateDummyAndUse();
	GL::LoadDummyExtensions();

	{
		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.hInstance = ::GetModuleHandle(NULL);
		windowClass.lpszClassName = "GamerWindow";
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hIcon = windowClass.hIconSm = ::LoadIcon(NULL, IDI_APPLICATION);
		::RegisterClassEx(&windowClass);
	}

	_window.Create("GamerWindow", "Window");

	dummy.Delete();

	_InitGL();
	_Init();

	MenuMain *menu = new MenuMain();
	menu->SetBounds(UIBounds(UICoord(0.5f, -256.f), UICoord(0.5f, -256.f), UICoord(0.f, 512.f), UICoord(0.f, 512.f)));
	menu->Initialise(Function<void, const String&>(*this, &Game::StartLevel), Callback(*this, &Game::ButtonQuit), _engine);
	menu->SetParent(&_ui);

	_window.Show();

	WindowEvent e;
	_running = true;
	while (_running)
	{
		_timer.Start();
		if (_deltaTime > 1 / 30.f)
			_deltaTime = 1 / 30.f;

		_engine.pInputManager->ClearMouseInput();

		while (_window.PollEvent(e))
		{
			switch (e.type)
			{
			case WindowEvent::CLOSED:
				_running = false;
				break;

			case WindowEvent::RESIZE:
				Resize(e.data.resize.w, e.data.resize.h);
				Render();
				break;

			case WindowEvent::MOUSEMOVE:
				MouseMove(e.data.mouseMove.x, e.data.mouseMove.y);
				break;

			case WindowEvent::LEFTMOUSEDOWN:
				MouseDown();
				break;

			case WindowEvent::LEFTMOUSEUP:
				MouseUp();
				break;

			case WindowEvent::KEYDOWN:
				if (e.data.keyDown.isRepeat)
					break;

				KeyDown(e.data.keyDown.key);
				break;

			case WindowEvent::KEYUP:
				KeyUp(e.data.keyUp.key);

				break;

			case WindowEvent::CHAR:
				InputChar(e.data.character);
				break;

			case WindowEvent::RAWINPUT:
				if (e.data.rawInput.type == WindowEvent::RawInputType::MOUSE)
				{
					_engine.pInputManager->AddMouseInput((float)e.data.rawInput.mouse.lastX, (float)e.data.rawInput.mouse.lastY);
				}
				break;

			case WindowEvent::FOCUS_LOST:
				_engine.pInputManager->Reset();
				break;
			}
		}

		Frame();
		_deltaTime = _timer.SecondsSinceStart();
	}
}

void Game::StartLevel(const String& filename)
{
	String ext = Utilities::GetExtension(filename);

	if (ext == ".txt")
	{
		_world.Clear();

		String fileString = IO::ReadFileString(filename.GetData());
		LevelGeneration::GenerateLevel(_world, fileString);
	}
	else
	{
		_world.Clear();
		_world.ReadObjects(filename.GetData());
	}

	OPlayer* player = _world.CreateObject<OPlayer>();
	
	OLight* light = _world.CreateObject<OLight>();
	light->SetParent(player, false);

	Transform spawnTransform(Vector3(0, 4.f, 0));
	player->SetRelativeTransform(spawnTransform);

	_uiIsActive = false;
}

void Game::Frame()
{
	_engine.pAudioManager->FillBuffer();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	_world.Update(_deltaTime);
	_ui.Update(_deltaTime);

	Render();
}

void Game::Render()
{
	_renderQueue.Clear();
	_uiQueue.Clear();

	const OCamera* activeCamera = GameInstance::Instance().GetActiveCamera();
	if (activeCamera)
	{
		Frustum cameraFrustum;
		activeCamera->GetProjection().ToFrustum(activeCamera->GetAbsoluteTransform(), cameraFrustum);
		activeCamera->Use(_renderQueue);
		_world.Render(_renderQueue, &cameraFrustum);
	}

	_uiQueue.CreateCameraEntry(_uiCamera, Transform(Vector3(_uiCamera.GetDimensions().x / 2.f, _uiCamera.GetDimensions().y / 2.f, 0.f)));

	if (_uiIsActive)
		_ui.Render(_uiQueue);

	if (_consoleIsActive)
		_engine.pConsole->Render(_uiQueue, *_engine.pFontManager->Get("consolas", _engine.context), _deltaTime);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);

	_shaderUnlit.Use();
	_uiQueue.Render(ERenderChannels::UNLIT, _engine.pMeshManager, _engine.pTextureManager, 0);

	if (activeCamera)
	{
		_shaderLit.Use();
		_shaderLit.SetInt(DefaultUniformVars::intCubemap, 100);
		_shaderLit.SetInt(DefaultUniformVars::intTextureDiffuse, 0);
		_shaderLit.SetInt(DefaultUniformVars::intTextureNormal, 1);
		_shaderLit.SetInt(DefaultUniformVars::intTextureSpecular, 2);
		_shaderLit.SetInt(DefaultUniformVars::intTextureReflection, 3);
		_reflect.Bind(100);
		_renderQueue.Render(ERenderChannels::SURFACE, _engine.pMeshManager, _engine.pTextureManager, _shaderLit.GetInt(DefaultUniformVars::intLightCount));
	
		_shaderUnlit.Use();
		_renderQueue.Render(ERenderChannels::UNLIT, _engine.pMeshManager, _engine.pTextureManager, 0);
	}

	if (ImGui::GetCurrentContext() && ImGui::GetCurrentContext()->WithinFrameScope)
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	_window.SwapBuffers();
}

void Game::Resize(uint16 w, uint16 h)
{
	GameInstance::Instance().OnResize(w, h);

	_ui.SetBounds(UIBounds(0.f, 0.f, UICoord(0.f, w), UICoord(0.f, h)));
	_uiCamera.SetDimensions(Vector2T(w, h));
}

void Game::MouseMove(uint16 x, uint16 y)
{
	Vector2 cp(x, y);
	cp /= _uiCamera.GetDimensions();
	cp.y = 1.f - cp.y;
	cp += Vector2(-0.5f, -0.5f);
	GameInstance::Instance().SetCursorPos(cp);

	if (_uiIsActive)
	{
		_ui.SetCursorPos((float)x, (float)(_uiCamera.GetDimensions().y - y));
	}
}

void Game::MouseUp()
{
	if (_uiIsActive)
	{
		_ui.KeyUp(EKeycode::MOUSE_LEFT);
	}
}

void Game::MouseDown()
{
	if (_uiIsActive)
	{
		_ui.KeyDown(EKeycode::MOUSE_LEFT);
	}
}

void Game::KeyDown(EKeycode key)
{
	if (_uiIsActive)
		_ui.KeyDown(key);

	_engine.pInputManager->KeyDown(key);

	if (key == EKeycode::TILDE)
	{
		_consoleIsActive = !_consoleIsActive;
		return;
	}
}

void Game::KeyUp(EKeycode key)
{
	if (_uiIsActive)
		_ui.KeyUp(key);
	
	_engine.pInputManager->KeyUp(key);
}

void Game::InputChar(char character)
{
	if (_uiIsActive)
		_ui.InputChar(character);

	if (_consoleIsActive)
	{
		if (character == '`')
			return;

		_engine.pConsole->InputChar(character, _engine.context);
	}
}

void Game::ButtonQuit()
{
	_running = false;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK Game::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui::GetCurrentContext())
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
			return TRUE;

		ImGuiIO& io = ImGui::GetIO();

		if (io.WantCaptureMouse && WindowFunctions_Win32::IsMouseInput(msg))
			return 0;
		if (io.WantCaptureKeyboard && WindowFunctions_Win32::IsKeyInput(msg))
			return 0;
	}

	return WindowFunctions_Win32::WindowProc(hwnd, msg, wparam, lparam);
}
