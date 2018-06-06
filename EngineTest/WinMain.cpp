#include <Windowsx.h>
#include <Engine\Camera.h>
#include <Engine\CubeRenderer.h>
#include <Engine\GL.h>
#include <Engine\GLProgram.h>
#include <Engine\InputManager.h>
#include <Engine\Light.h>
#include <Engine\ModelManager.h>
#include <Engine\Renderable.h>
#include <Engine\Skybox.h>
#include <Engine\String.h>
#include <Engine\TextureManager.h>
#include <Engine\Timer.h>
#include <Engine\Utilities.h>
#include <Engine\Window.h>

bool running = false;

CubeRenderer cubeRenderer;
InputManager inputManager;
ModelManager modelManager;
TextureManager textureManager;

//
GameObject lightParent1;
Vector3 rotationOffset1(0.f, 45, 0.f);
GameObject lightParent2;
Vector3 rotationOffset2(-11, -30, 0.f);
GameObject lightParent3;
Vector3 rotationOffset3(33, -2, 0.f);
GameObject lightParent4;
Vector3 rotationOffset4(15, 12.88f, 0);
Light light1;
Light light2;
Light light3;
Light light4;

Renderable renderable;
Camera camera;
Renderable cube;
GameObject cubeParent;
Vector3 cubeParentRotationOffset(2.f, 11.7f, 0.f);
Vector3 cubeRotationOffset(9.f, 23.f, 0.f);
//

Skybox sky;

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

		glViewport(0, 0, w, h);

		camera.SetAspectRatio((float)w / (float)h);

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

		inputManager.KeyDown((Keycode)wParam);
		break;
	case WM_KEYUP:

		inputManager.KeyUp((Keycode)wParam);
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

	window.Create(className, "Window", nullptr);
	window.UseGLContext();
	window.Show();

	//
	//Initialise OpenGL
	window.SetTitle("GL Init");
	//
	GL::LoadExtensions(window.GetHDC());
	wglSwapIntervalEXT(0);
	glClearColor(0.3f, 0.4f, 0.3f, 1.f);
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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
	textureManager.Initialise();
	cubeRenderer.Initialise();

	inputManager.BindKeyAxis(Keycode::A, &axisX, -1);
	inputManager.BindKeyAxis(Keycode::D, &axisX, 1);
	inputManager.BindKeyAxis(Keycode::LCTRL, &axisY, -1);
	inputManager.BindKeyAxis(Keycode::SPACE, &axisY, 1);
	inputManager.BindKeyAxis(Keycode::S, &axisZ, -1);
	inputManager.BindKeyAxis(Keycode::W, &axisZ, 1);

	inputManager.BindKeyAxis(Keycode::DOWN, &lookX, -1);
	inputManager.BindKeyAxis(Keycode::UP, &lookX, 1);
	inputManager.BindKeyAxis(Keycode::LEFT, &lookY, -1);
	inputManager.BindKeyAxis(Keycode::RIGHT, &lookY, 1);
	inputManager.BindKeyAxis(Keycode::Q, &lookX, -1);
	inputManager.BindKeyAxis(Keycode::E, &lookX, 1);

	inputManager.BindAxis(AxisType::MOUSE_X, &lookY);
	inputManager.BindAxis(AxisType::MOUSE_Y, &lookX);

	//
	//Load resources
	//
	window.SetTitle("Loading Models...");
	modelManager.LoadModel("Data/Model.obj", "MODEL");

	window.SetTitle("Loading Textures...");
	textureManager.LoadTexture("Data/Diffuse.png", "DIFFUSE");
	textureManager.LoadTexture("Data/Normal.png", "NORMAL");
	textureManager.LoadTexture("Data/Specular.png", "SPECULAR");
	textureManager.LoadTexture("Data/Reflectivity.png", "REFLECTION");
	
	const char *faces[6] = { "Data/SkyLeft.png", "Data/SkyRight.png", "Data/SkyUp.png", "Data/SkyDown.png", "Data/SkyFront.png", "Data/SkyBack.png" };
	sky.Load(faces);

	//
	//World setup
	window.SetTitle("Creating World...");
	//
	camera.transform.SetPosition(Vector3(0.f, 0.f, -3.f));

	light1.transform.SetPosition(Vector3(0.f, 0.f, -4.f));
	light2.transform.SetPosition(Vector3(0.f, 0.f, -3.5f));
	light3.transform.SetPosition(Vector3(0.f, 0.f, -3.f));
	light4.transform.SetPosition(Vector3(0.f, 0.f, -2.f));

	light1.SetColour(Vector3(1.f, 0.f, 0.f));
	light2.SetColour(Vector3(0.f, 1.f, 0.f));
	light3.SetColour(Vector3(0.f, 0.f, 1.f));

	light1.SetRadius(5.f);
	light2.SetRadius(5.f);
	light3.SetRadius(5.f);
	light4.SetRadius(10.f);

	light1.SetParent(lightParent1);
	light2.SetParent(lightParent2);
	light3.SetParent(lightParent3);
	light4.SetParent(lightParent4);
	lightParent4.SetParent(cube);

	cube.transform.SetScale(Vector3(.5f, .5f, .5f));
	cube.transform.SetPosition(Vector3(2.f, 0.f, 0.f));
	cube.SetParent(cubeParent);
	cube.SetRenderer(cubeRenderer);

	renderable.SetParent(cubeParent);
	renderable.SetRenderer(*modelManager.GetRenderer("MODEL"));

	window.SetTitle("Window");
	
	//
	//Windows message loop
	//
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
		inputManager.MouseMove(cursorPos.x - lockPos.x, cursorPos.y - lockPos.y);
		::SetCursorPos(lockPos.x, lockPos.y);
	}

	float moveAmount = MOVERATE * dt;
	float turnAmount = TURNRATE * dt;

	camera.transform.Move(camera.transform.GetForwardVector() * axisZ * moveAmount +
		camera.transform.GetRightVector() * axisX * moveAmount +
		camera.transform.GetUpVector() * axisY * moveAmount);

	camera.transform.Rotate(Vector3(lookX * turnAmount, lookY * turnAmount, lookZ * turnAmount));

	lightParent1.transform.Rotate(rotationOffset1 * dt);
	lightParent2.transform.Rotate(rotationOffset2 * dt);
	lightParent3.transform.Rotate(rotationOffset3 * dt);
	lightParent4.transform.Rotate(rotationOffset4 * dt);

	cubeParent.transform.Rotate(cubeParentRotationOffset * dt);
	cube.transform.Rotate(cubeRotationOffset * dt);

	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//
		program_Unlit.Use();
		program_Unlit.SetMat4("M_Projection", camera.GetProjectionMatrix());
		program_Unlit.SetMat4("M_View", camera.MakeInverseTransformationMatrix());

		light1.DebugRender(cubeRenderer);
		light2.DebugRender(cubeRenderer);
		light3.DebugRender(cubeRenderer);
		light4.DebugRender(cubeRenderer);
		//

		//
		program_Lit.Use();
		program_Lit.SetMat4("M_Projection", camera.GetProjectionMatrix());
		program_Lit.SetMat4("M_View", camera.MakeInverseTransformationMatrix());
		program_Lit.SetInt("Cubemap", UNIT_CUBEMAP);
		program_Lit.SetInt("T_Diffuse", UNIT_DIFFUSE);
		program_Lit.SetInt("T_Normal", UNIT_NORMAL);
		program_Lit.SetInt("T_Specular", UNIT_SPECULAR);
		program_Lit.SetInt("T_Reflectivity", UNIT_REFLECTION);

		light1.ToShader(0);
		light2.ToShader(1);
		light3.ToShader(2);
		light4.ToShader(3);

		sky.Bind(UNIT_CUBEMAP);
		textureManager.GetTexture("DIFFUSE")->Bind(UNIT_DIFFUSE);
		textureManager.GetTexture("NORMAL")->Bind(UNIT_NORMAL);
		textureManager.GetTexture("SPECULAR")->Bind(UNIT_SPECULAR);
		textureManager.GetTexture("REFLECTION")->Bind(UNIT_REFLECTION);
		
		cube.Render();

		textureManager.UVDefault().Bind(UNIT_NORMAL);
		textureManager.White().Bind(UNIT_SPECULAR);
		textureManager.White().Bind(UNIT_REFLECTION);
		renderable.Render();
		//

		//
		program_Sky.Use();
		Mat4 skyView = camera.MakeInverseTransformationMatrix();
		skyView[3][0] = skyView[3][1] = skyView[3][2] = 0;

		program_Sky.SetMat4("M_Projection", camera.GetProjectionMatrix());
		program_Sky.SetMat4("M_View", skyView);

		glDepthFunc(GL_LEQUAL);
		sky.Render(cubeRenderer);
		glDepthFunc(GL_LESS);
		//

		window.SwapBuffers();
	}

	dt = timer.SecondsSinceStart();
	t += dt;
}
