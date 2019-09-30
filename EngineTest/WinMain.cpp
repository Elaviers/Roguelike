#include <Windowsx.h>
#include <Engine\DrawUtils.hpp>
#include <Engine\Font.hpp>
#include <Engine\FontManager.hpp>
#include <Engine\GL.hpp>
#include <Engine\GLContext.hpp>
#include <Engine\GLProgram.hpp>
#include <Engine\InputManager.hpp>
#include <Engine\ObjCamera.hpp>
#include <Engine\ObjLight.hpp>
#include <Engine\ObjRenderable.hpp>
#include <Engine\ModelManager.hpp>
#include <Engine\Skybox.hpp>
#include <Engine\StackAllocator.hpp>
#include <Engine\String.hpp>
#include <Engine\TextureManager.hpp>
#include <Engine\Timer.hpp>
#include <Engine\Utilities.hpp>
#include <Engine\Window.hpp>

#define ORTHO 0

const char* fontName = "consolas.txt";
const char *modelName = "Model";
const char *texDiffuse = "Diffuse.png";
const char *texNormal = "Normal.png";
const char *texSpecular = "Specular.png";
const char *texReflection = "Reflectivity.png";
const char *skyFaces[6] = { "SkyLeft.png", "SkyRight.png", "SkyUp.png", "SkyDown.png", "SkyFront.png", "SkyBack.png" };

bool running = false;

//
GameObject lightParent1;
Vector3 rotationOffset1(0.f, 45, 0.f);
GameObject lightParent2;
Vector3 rotationOffset2(-11, -30, 0.f);
GameObject lightParent3;
Vector3 rotationOffset3(33, -2, 0.f);
GameObject lightParent4;
Vector3 rotationOffset4(15, 12.88f, 0);
ObjLight light1;
ObjLight light2;
ObjLight light3;
ObjLight light4;

ObjRenderable renderable;
ObjCamera camera;
ObjRenderable cube;
GameObject cubeParent;
Vector3 cubeParentRotationOffset(2.f, 11.7f, 0.f);
Vector3 cubeRotationOffset(9.f, 23.f, 0.f);

Mat4 cameraMatrix;
//

Skybox sky;

GLContext glContext;

GLProgram program_Lit;
GLProgram program_Unlit;
GLProgram program_Sky;
Window window;

bool cursorLocked = false;
POINT lockPos;

float axisX = 0.f, axisY = 0.f, axisZ = 0.f;
float lookX = 0.f, lookY = 0.f, lookZ = 0.f;

Timer timer;
float dt = 0.f;
float t = 0;

#define TURNRATE 90.f //DEGS/SEC
#define MOVERATE 2.f //UNITS/SEC

void Frame();

inline void LockCursor()
{
	cursorLocked = true;
	::ShowCursor(FALSE);
	::GetCursorPos(&lockPos);
}

inline void UnlockCursor()
{
	cursorLocked = false;
	::ShowCursor(TRUE);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CLOSE:
		::DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		running = false;
		break;

	case WM_SIZE:
	{
		uint16 w = LOWORD(lParam);
		uint16 h = HIWORD(lParam);

		camera.SetViewport(w, h);
		cameraMatrix = Matrix::Ortho(0, w, 0, h, -100.f, 100.f, 1.f);

		if (running) Frame(); //Please pretend this line doesn't exist
	}
		break;

	case WM_LBUTTONDOWN:
		if (cursorLocked)
			UnlockCursor();
		else
			LockCursor();
		break;

	case WM_SETFOCUS:
		//Focus gained
		break;
	case WM_KILLFOCUS:
		if (cursorLocked)
			UnlockCursor();
		break;

	case WM_KEYDOWN:
		if (lParam & (1 << 30))
			break; //Key repeats ignored

		Engine::Instance().pInputManager->KeyDown((Keycode)wParam);
		break;
	case WM_KEYUP:

		Engine::Instance().pInputManager->KeyUp((Keycode)wParam);
		break;

	default:
		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR cmdString, int cmdShow)
{
	//
	//Create window
	//
	LPCTSTR className = TEXT("WINDOWCLASS");

	WNDCLASSEX windowClass = {
		sizeof(WNDCLASSEX),
		0,
		WindowProc,
		0,
		0,
		instance,
		::LoadIcon(NULL, IDI_INFORMATION),
		::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		className,
		::LoadIcon(NULL, IDI_INFORMATION)
	};
	::RegisterClassEx(&windowClass);

	GLContext dummy;
	dummy.CreateDummyAndUse(className);
	GL::LoadDummyExtensions();

	window.Create(className, "Window", nullptr);

	dummy.Delete();

	glContext.Create(window);
	glContext.Use(window);
	window.Show();

	//
	//Initialise OpenGL
	window.SetTitle("GL Init");
	//
	GL::LoadExtensions(window.GetHDC());
	wglSwapIntervalEXT(0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glFrontFace(GL_CCW);

	glLineWidth(2);
	glPointSize(8);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//
	//Compile & link shaders
	window.SetTitle("Compiling Shaders...");
	//
	program_Lit.Load("Data/Shader.vert", "Data/Phong.frag");
	program_Unlit.Load("Data/Shader.vert", "Data/Unlit.frag");
	program_Sky.Load("Data/Sky.vert", "Data/Sky.frag");

	//
	//Initialise managers
	window.SetTitle("Manager Init");

	//
	Engine::Instance().Init(ENG_ALL);
	Engine::Instance().pFontManager->AddPath(Utilities::GetSystemFontDir());

	ModelManager& modelManager = *Engine::Instance().pModelManager;
	TextureManager& textureManager = *Engine::Instance().pTextureManager;
	InputManager& inputManager = *Engine::Instance().pInputManager;

	modelManager.SetRootPath("Data/");
	textureManager.SetRootPath("Data/");
	Engine::Instance().pFontManager->SetRootPath("Data/");

	inputManager.BindKeyAxis(Keycode::A, &axisX, -1);
	inputManager.BindKeyAxis(Keycode::D, &axisX, 1);
	inputManager.BindKeyAxis(Keycode::F, &axisY, -1);
	inputManager.BindKeyAxis(Keycode::SPACE, &axisY, 1);
	inputManager.BindKeyAxis(Keycode::S, &axisZ, -1);
	inputManager.BindKeyAxis(Keycode::W, &axisZ, 1);

	inputManager.BindKeyAxis(Keycode::DOWN, &lookX, -1);
	inputManager.BindKeyAxis(Keycode::UP, &lookX, 1);
	inputManager.BindKeyAxis(Keycode::LEFT, &lookY, -1);
	inputManager.BindKeyAxis(Keycode::RIGHT, &lookY, 1);
	inputManager.BindKeyAxis(Keycode::Q, &lookZ, -1);
	inputManager.BindKeyAxis(Keycode::E, &lookZ, 1);

	inputManager.BindAxis(AxisType::MOUSE_X, &lookY);
	inputManager.BindAxis(AxisType::MOUSE_Y, &lookX);


	//
	//Load resources
	//
	window.SetTitle("Loading Models...");
	modelManager.Get(modelName);

	window.SetTitle("Loading Material Textures...");
	textureManager.Get(texDiffuse);
	textureManager.Get(texNormal);
	textureManager.Get(texSpecular);
	textureManager.Get(texReflection);

	window.SetTitle("Loading Skybox Textures...");
	sky.Load(skyFaces);

	window.SetTitle("Loading Fonts...");
	Engine::Instance().pFontManager->Get(fontName);
	//
	//World setup
	window.SetTitle("Creating World...");
	//

	if (ORTHO)
	{
		camera.SetProjectionType(ProjectionType::ORTHOGRAPHIC);
		camera.SetZBounds(-100000.f, 100000.f);
		camera.SetScale(128.f);
		camera.SetRelativeRotation(Vector3(-90.f, 0.f, 0.f));
	}
	else
	{
		camera.SetProjectionType(ProjectionType::PERSPECTIVE);
		camera.SetZBounds(0.001f, 100000.f);
		camera.SetRelativePosition(Vector3(0.f, 0.f, -5.f));
	}

	light1.SetRelativePosition(Vector3(0.f, 0.f, -4.f));
	light2.SetRelativePosition(Vector3(0.f, 0.f, -3.5f));
	light3.SetRelativePosition(Vector3(0.f, 0.f, -3.f));
	light4.SetRelativePosition(Vector3(0.f, 0.f, -2.f));

	light1.SetColour(Vector3(1.f, 0.f, 0.f));
	light2.SetColour(Vector3(0.f, 1.f, 0.f));
	light3.SetColour(Vector3(0.f, 0.f, 1.f));

	light1.SetRadius(5.f);
	light2.SetRadius(5.f);
	light3.SetRadius(5.f);
	light4.SetRadius(10.f);

	light1.SetParent(&lightParent1);
	light2.SetParent(&lightParent2);
	light3.SetParent(&lightParent3);
	light4.SetParent(&lightParent4);
	lightParent4.SetParent(&cube);

	cube.SetRelativePosition(Vector3(2.f, 0.f, 0.f));
	cube.SetParent(&cubeParent);
	cube.SetModel(&modelManager.Cube());

	renderable.SetParent(&cubeParent);
	renderable.SetModel(modelName);

	window.SetTitle("Window");

	//
	//Windows message loop
	running = true;
	MSG msg;
	while (running)
	{
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		Frame();
	}

	return 0;
}

enum TextureUnit
{
	UNIT_CUBEMAP = 0,
	UNIT_DIFFUSE = 1,
	UNIT_NORMAL = 2,
	UNIT_SPECULAR = 3,
	UNIT_REFLECTION = 4
};

void Frame()
{
	timer.Start();

	if (cursorLocked) {
		POINT cursorPos;
		::GetCursorPos(&cursorPos);
		Engine::Instance().pInputManager->MouseMove((short)(cursorPos.x - lockPos.x), (short)(cursorPos.y - lockPos.y));
		::SetCursorPos(lockPos.x, lockPos.y);
	}

	float moveAmount = MOVERATE * dt;
	float turnAmount = TURNRATE * dt;

	camera.RelativeMove(camera.GetRelativeTransform().GetForwardVector() * axisZ * moveAmount +
		camera.GetRelativeTransform().GetRightVector() * axisX * moveAmount +
		camera.GetRelativeTransform().GetUpVector() * axisY * moveAmount);

	camera.AddRelativeRotation(Vector3(lookX * turnAmount, lookY * turnAmount, lookZ * turnAmount));

	lightParent1.RelativeRotate(rotationOffset1 * dt);
	lightParent2.RelativeRotate(rotationOffset2 * dt);
	lightParent3.RelativeRotate(rotationOffset3 * dt);
	lightParent4.RelativeRotate(rotationOffset4 * dt);

	cubeParent.RelativeRotate(cubeParentRotationOffset * dt);
	cube.RelativeRotate(cubeRotationOffset * dt);

	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//
		program_Lit.Use();
		camera.Use();
		program_Lit.SetInt(DefaultUniformVars::intCubemap, UNIT_CUBEMAP);
		program_Lit.SetInt(DefaultUniformVars::intTextureDiffuse, UNIT_DIFFUSE);
		program_Lit.SetInt(DefaultUniformVars::intTextureNormal, UNIT_NORMAL);
		program_Lit.SetInt(DefaultUniformVars::intTextureSpecular, UNIT_SPECULAR);
		program_Lit.SetInt(DefaultUniformVars::intTextureReflection, UNIT_REFLECTION);

		light1.ToShader(0);
		light2.ToShader(1);
		light3.ToShader(2);
		light4.ToShader(3);

		ModelManager& modelManager = *Engine::Instance().pModelManager;
		TextureManager& textureManager = *Engine::Instance().pTextureManager;

		sky.Bind(UNIT_CUBEMAP);
		textureManager.Get(texDiffuse)->Bind(UNIT_DIFFUSE);
		textureManager.Get(texNormal)->Bind(UNIT_NORMAL);
		textureManager.Get(texSpecular)->Bind(UNIT_SPECULAR);
		textureManager.Get(texReflection)->Bind(UNIT_REFLECTION);
		
		cube.Render(RenderChannel::ALL);

		textureManager.NormalDefault()->Bind(UNIT_NORMAL);
		textureManager.White()->Bind(UNIT_SPECULAR);
		textureManager.White()->Bind(UNIT_REFLECTION);
		renderable.Render(RenderChannel::ALL);
		//

		//
		if (camera.GetProjectionType() == ProjectionType::PERSPECTIVE)
		{
			program_Sky.Use();
			Mat4 skyView = camera.GetInverseTransformationMatrix();
			skyView[3][0] = skyView[3][1] = skyView[3][2] = 0;

			program_Sky.SetMat4(DefaultUniformVars::mat4Projection, camera.GetProjectionMatrix());
			program_Sky.SetMat4(DefaultUniformVars::mat4View, skyView);

			glDepthFunc(GL_LEQUAL);
			sky.Render(modelManager);
			glDepthFunc(GL_LESS);
		}
		//

		//
		{
			float vpScale = camera.GetProjectionType() == ProjectionType::ORTHOGRAPHIC ? 1.f : 10.f;
			float w = 2.f;

			program_Unlit.Use();
			camera.Use();

			textureManager.White()->Bind(0);

			light1.Render(RenderChannel::ALL);
			light2.Render(RenderChannel::ALL);
			light3.Render(RenderChannel::ALL);
			light4.Render(RenderChannel::ALL);

			glDisable(GL_CULL_FACE);
			
			program_Unlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 0.f, 0.f, 1.f));
			DrawUtils::DrawGrid(modelManager, camera, Direction::FORWARD, w, 1.f, vpScale);

			program_Unlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(0.f, 1.f, 0.f, 1.f));
			DrawUtils::DrawGrid(modelManager, camera, Direction::UP, w, 1.f, vpScale);

			program_Unlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(0.f, 0.f, 1.f, 1.f));
			DrawUtils::DrawGrid(modelManager, camera, Direction::RIGHT, w, 1.f, vpScale);

			glEnable(GL_CULL_FACE);


			//UI
			glDepthFunc(GL_ALWAYS);
			program_Unlit.SetMat4(DefaultUniformVars::mat4Projection, cameraMatrix);
			program_Unlit.SetMat4(DefaultUniformVars::mat4View, Matrix::Identity());
			program_Unlit.SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 1.f, 1.f, 1.f));

			static StackAllocator ftAllocator(1024);
			static String fpsString;
			static float timeTillUpdate = 0.f;
			static int sampledFrameCount = 0;

			timeTillUpdate -= dt;
			
			*ftAllocator.Push<float>() = 1.f / dt;
			sampledFrameCount++;

			if (timeTillUpdate <= 0.f)
			{
				auto ff = ftAllocator.GetFirstFrame();

				if (ff)
				{
					float avg = 0.f;
					for (int i = 0; i < sampledFrameCount; ++i)
					{
						avg += *(float*)ff;
						ff += sizeof(float) + StackAllocator::GetOverheadBytesPerFrame();
					}

					avg /= sampledFrameCount;

					fpsString = String::From(avg);
				}

				ftAllocator.Reset();
				timeTillUpdate = 0.05f;
				sampledFrameCount = 0;
			}

			Transform fontTransform(Vector3(0, 0, 0), Rotation(), Vector3(64, 0, 0));
			Engine::Instance().pFontManager->Get(fontName)->RenderString(fpsString.GetData(), fontTransform);

			glDepthFunc(GL_LESS);
		}
		//
		window.SwapBuffers();
	}

	dt = timer.SecondsSinceStart();
	t += dt;
}
