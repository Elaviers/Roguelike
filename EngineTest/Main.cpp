#include <Engine/EngineInstance.hpp>
#include <Engine/EntCamera.hpp>
#include <Engine/EntLight.hpp>
#include <Engine/EntRenderable.hpp>
#include <ELCore/StackAllocator.hpp>
#include <ELGraphics/FontManager.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELGraphics/Skybox.hpp>
#include <ELSys/Entry.hpp>
#include <ELSys/GLContext.hpp>
#include <ELSys/GLProgram.hpp>
#include <ELSys/InputManager.hpp>
#include <ELSys/Timer.hpp>
#include <ELSys/Utilities.hpp>

#define ORTHO 0

const char* fontName = "consolas.txt";
const char *meshName = "Model";
const char *texDiffuse = "Diffuse.png";
const char *texNormal = "Normal.png";
const char *texSpecular = "Specular.png";
const char *texReflection = "Reflectivity.png";
const char *skyFaces[6] = { "SkyLeft.png", "SkyRight.png", "SkyUp.png", "SkyDown.png", "SkyFront.png", "SkyBack.png" };

bool running = false;

//
EngineInstance engine;
RenderQueue renderQueue;
RenderQueue uiQueue;

Entity lightParent1;
Vector3 rotationOffset1(0.f, 45, 0.f);
Entity lightParent2;
Vector3 rotationOffset2(-11, -30, 0.f);
Entity lightParent3;
Vector3 rotationOffset3(33, -2, 0.f);
Entity lightParent4;
Vector3 rotationOffset4(15, 12.88f, 0);
EntLight light1;
EntLight light2;
EntLight light3;
EntLight light4;

EntRenderable renderable;
EntCamera camera;
EntRenderable cube;
Entity cubeParent;
Vector3 cubeParentRotationOffset(2.f, 11.7f, 0.f);
Vector3 cubeRotationOffset(9.f, 23.f, 0.f);

Matrix4 uiProjectionMatrix;

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
float mouseLookX = 0.f, mouseLookY = 0.f;

Timer timer;
float dt = 0.f;
float t = 0;

#define TURNRATE 90.f //DEGS/SEC
#define MOVERATE 2.f //UNITS/SEC
#define MOUSESENS 0.1f

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

int Main()
{
	GLContext dummy;
	dummy.CreateDummyAndUse();
	GL::LoadDummyExtensions();

	window.Create("Window", nullptr);

	dummy.Delete();

	glContext.Create(window);
	glContext.Use(window);
	window.Show();

	//
	//Initialise OpenGL
	window.SetTitle("GL Init");
	//
	GL::LoadExtensions(window);
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
	engine.Init(EEngineCreateFlags::ALL, nullptr);
	engine.pFontManager->AddPath(Utilities::GetSystemFontDir());

	ModelManager& modelManager = *engine.pModelManager;
	TextureManager& textureManager = *engine.pTextureManager;
	InputManager& inputManager = *engine.pInputManager;

	engine.pMaterialManager->SetRootPath("Data/");
	engine.pMeshManager->SetRootPath("Data/");
	modelManager.SetRootPath("Data/");
	textureManager.SetRootPath("Data/");
	engine.pFontManager->SetRootPath("Data/");

	inputManager.BindKeyAxis(EKeycode::A, &axisX, -1);
	inputManager.BindKeyAxis(EKeycode::D, &axisX, 1);
	inputManager.BindKeyAxis(EKeycode::F, &axisY, -1);
	inputManager.BindKeyAxis(EKeycode::SPACE, &axisY, 1);
	inputManager.BindKeyAxis(EKeycode::S, &axisZ, -1);
	inputManager.BindKeyAxis(EKeycode::W, &axisZ, 1);

	inputManager.BindKeyAxis(EKeycode::DOWN, &lookX, -1);
	inputManager.BindKeyAxis(EKeycode::UP, &lookX, 1);
	inputManager.BindKeyAxis(EKeycode::LEFT, &lookY, -1);
	inputManager.BindKeyAxis(EKeycode::RIGHT, &lookY, 1);
	inputManager.BindKeyAxis(EKeycode::Q, &lookZ, -1);
	inputManager.BindKeyAxis(EKeycode::E, &lookZ, 1);

	inputManager.BindAxis(EAxis::MOUSE_X, &mouseLookY);
	inputManager.BindAxis(EAxis::MOUSE_Y, &mouseLookX);


	//
	//Load resources
	//
	window.SetTitle("Loading Models...");
	modelManager.Get(meshName, engine.context);

	window.SetTitle("Loading Material Textures...");
	textureManager.Get(texDiffuse, engine.context);
	textureManager.Get(texNormal, engine.context);
	textureManager.Get(texSpecular, engine.context);
	textureManager.Get(texReflection, engine.context);

	window.SetTitle("Loading Skybox Textures...");
	sky.Load(skyFaces, engine.context);

	window.SetTitle("Loading Fonts...");
	engine.pFontManager->Get(fontName, engine.context);
	//
	//World setup
	window.SetTitle("Creating World...");
	//

	if (ORTHO)
	{
		camera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
		camera.GetProjection().SetNearFar(-100000.f, 100000.f);
		camera.GetProjection().SetOrthographicScale(128.f);
		camera.SetRelativeRotation(Vector3(-90.f, 0.f, 0.f));
	}
	else
	{
		camera.GetProjection().SetType(EProjectionType::PERSPECTIVE);
		camera.GetProjection().SetNearFar(0.001f, 100000.f);
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
	cube.SetModel(modelManager.Cube(), engine.context);
	cube.SetMaterial("bricks", engine.context);

	renderable.SetParent(&cubeParent);
	renderable.SetModel(meshName, engine.context);

	window.SetTitle("Window");

	//
	//Event loop
	//
	running = true;
	WindowEvent e;
	while (running)
	{
		while (window.PollEvent(e))
		{
			switch (e.type)
			{
			case WindowEvent::CLOSED:
				running = false;
				break;

			case WindowEvent::RESIZE:
			{
				const uint16& w = e.data.resize.w;
				const uint16& h = e.data.resize.h;

				camera.GetProjection().SetDimensions(Vector2T(w, h));
				uiProjectionMatrix = Matrix4::ProjectionOrtho(0.f, w, 0.f, h, -100.f, 100.f, 1.f);

				if (running) Frame(); //Please pretend this line doesn't exist
			}
			break;

			case WindowEvent::FOCUS_LOST:
				if (cursorLocked)
					UnlockCursor();
				break;

			case WindowEvent::LEFTMOUSEDOWN:
				if (cursorLocked)
					UnlockCursor();
				else
					LockCursor();
				break;

			case WindowEvent::KEYDOWN:
				if (!e.data.keyDown.isRepeat)
					engine.pInputManager->KeyDown(e.data.keyDown.key);
				break;
			case WindowEvent::KEYUP:
				engine.pInputManager->KeyUp(e.data.keyUp.key);
				break;

			}
		}

		Frame();
	}

	return 0;

}

void Frame()
{
	timer.Start();

	if (cursorLocked) {
		POINT cursorPos;
		::GetCursorPos(&cursorPos);
		engine.pInputManager->ClearMouseInput();
		engine.pInputManager->AddMouseInput((short)(cursorPos.x - lockPos.x), (short)(cursorPos.y - lockPos.y));
		::SetCursorPos(lockPos.x, lockPos.y);
	}

	float moveAmount = MOVERATE * dt;
	float turnAmount = TURNRATE * dt;

	camera.RelativeMove(camera.GetRelativeTransform().GetForwardVector() * axisZ * moveAmount +
		camera.GetRelativeTransform().GetRightVector() * axisX * moveAmount +
		camera.GetRelativeTransform().GetUpVector() * axisY * moveAmount);

	camera.AddRelativeRotation(Vector3(lookX * turnAmount + mouseLookX * MOUSESENS, lookY * turnAmount + mouseLookY * MOUSESENS, lookZ * turnAmount));

	lightParent1.RelativeRotate(rotationOffset1 * dt);
	lightParent2.RelativeRotate(rotationOffset2 * dt);
	lightParent3.RelativeRotate(rotationOffset3 * dt);
	lightParent4.RelativeRotate(rotationOffset4 * dt);

	cubeParent.RelativeRotate(cubeParentRotationOffset * dt);
	cube.RelativeRotate(cubeRotationOffset * dt);

	//Update FPS counter
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
	
	//
	//Now render the frame
	//
	{
		//Prepare queue
		renderQueue.ClearDynamicQueue();
		uiQueue.ClearDynamicQueue();

		light1.Render(renderQueue);
		light2.Render(renderQueue);
		light3.Render(renderQueue);
		light4.Render(renderQueue);
		sky.Render(renderQueue);
		cube.Render(renderQueue);
		renderable.Render(renderQueue);

		//grid
		float limit = camera.GetProjection().GetType() == EProjectionType::ORTHOGRAPHIC ? 1.f : 10.f;
		RenderEntry& e = renderQueue.NewDynamicEntry(ERenderChannels::UNLIT);
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
		e.AddSetColour(Colour::Red);
		e.AddGrid(camera.GetWorldTransform(), camera.GetProjection(), EDirection::RIGHT, 1.f, limit, 0.f);
		e.AddSetColour(Colour::Green);
		e.AddGrid(camera.GetWorldTransform(), camera.GetProjection(), EDirection::UP, 1.f, limit, 0.f);
		e.AddSetColour(Colour::Blue);
		e.AddGrid(camera.GetWorldTransform(), camera.GetProjection(), EDirection::FORWARD, 1.f, limit, 0.f);

		//UI
		//fps string
		static Transform fontTransform(Vector3(0, 0, 0), Rotation(), Vector3(64, 0, 0));
		RenderEntry& strEntry = uiQueue.NewDynamicEntry(ERenderChannels::UNLIT);
		strEntry.AddSetColour(Colour::Yellow);
		engine.pFontManager->Get(fontName, engine.context)->RenderString(strEntry, fpsString.GetData(), fontTransform);

		//Begin drawing
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//Draw unlit
		{
			program_Unlit.Use();
			camera.Use();
			renderQueue.Render(ERenderChannels::UNLIT | ERenderChannels::EDITOR, *engine.pMeshManager, *engine.pTextureManager);
		}

		{
			program_Lit.Use();
			camera.Use();
			program_Lit.SetInt(DefaultUniformVars::intCubemap, 100);
			program_Lit.SetInt(DefaultUniformVars::intTextureDiffuse, 0);
			program_Lit.SetInt(DefaultUniformVars::intTextureNormal, 1);
			program_Lit.SetInt(DefaultUniformVars::intTextureSpecular, 2);
			program_Lit.SetInt(DefaultUniformVars::intTextureReflection, 3);
			renderQueue.Render(ERenderChannels::SURFACE, *engine.pMeshManager, *engine.pTextureManager);
		}

		//Draw sky
		if (camera.GetProjection().GetType() == EProjectionType::PERSPECTIVE)
		{
			program_Sky.Use();
			Matrix4 skyView = camera.GetInverseTransformationMatrix();
			skyView[3][0] = skyView[3][1] = skyView[3][2] = 0;

			program_Sky.SetMatrix4(DefaultUniformVars::mat4Projection, camera.GetProjection().GetMatrix());
			program_Sky.SetMatrix4(DefaultUniformVars::mat4View, skyView);

			glDepthFunc(GL_LEQUAL);
			renderQueue.Render(ERenderChannels::SKY, *engine.pMeshManager, *engine.pTextureManager);
			glDepthFunc(GL_LESS);
		}

		//Draw UI
		{
			program_Unlit.Use();
			program_Unlit.SetMatrix4(DefaultUniformVars::mat4Projection, uiProjectionMatrix);
			program_Unlit.SetMatrix4(DefaultUniformVars::mat4View, Matrix4::Identity());
			uiQueue.Render(ERenderChannels::UNLIT, *engine.pMeshManager, *engine.pTextureManager);
		}
			
		//
		window.SwapBuffers();
	}

	dt = timer.SecondsSinceStart();
	t += dt;
}
