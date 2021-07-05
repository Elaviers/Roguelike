#include <Engine/EngineInstance.hpp>
#include <Engine/OCamera.hpp>
#include <Engine/OLight.hpp>
#include <Engine/ORenderable.hpp>
#include <Engine/World.hpp>
#include <ELCore/StackAllocator.hpp>
#include <ELGraphics/FontManager.hpp>
#include <ELGraphics/Material_Surface.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELGraphics/Skybox.hpp>
#include <ELSys/Entry.hpp>
#include <ELSys/GLContext.hpp>
#include <ELSys/GLProgram.hpp>
#include <ELSys/InputManager.hpp>
#include <ELSys/Timer.hpp>
#include <ELSys/Utilities.hpp>

//Orthographic camera mode
#define ORTHO 0

//Names of a bunch of assets
constexpr const char *fontName = "consolas.txt";
constexpr const char *meshName = "Model";
constexpr const char *texDiffuse = "Diffuse.png";
constexpr const char *texNormal = "Normal.png";
constexpr const char *texSpecular = "Specular.png";
constexpr const char *texReflection = "Reflectivity.png";
const char *skyFaces[6] = { "SkyLeft.png", "SkyRight.png", "SkyUp.png", "SkyDown.png", "SkyFront.png", "SkyBack.png" };

//Used by main loop
bool running = false;

//An instance of the roguelike engine.
//Contains asset managers mostly
EngineInstance engine;
RenderQueue renderQueue;
RenderQueue uiQueue;
World world(&engine.context);

//LIGHTS
WorldObject* lightParent1;
Vector3 rotationOffset1(0.f, 45, 0.f);
WorldObject* lightParent2;
Vector3 rotationOffset2(-11, -30, 0.f);
WorldObject* lightParent3;
Vector3 rotationOffset3(33, -2, 0.f);
WorldObject* lightParent4;
Vector3 rotationOffset4(15, 12.88f, 0);
OLight* light1;
OLight* light2;
OLight* light3;
OLight* light4;

//Other entities
ORenderable* renderable;
OCamera* camera;
ORenderable* cube;
WorldObject* cubeParent;
Vector3 cubeParentRotationOffset(2.f, 11.7f, 0.f);
Vector3 cubeRotationOffset(9.f, 23.f, 0.f);

//4x4 projection matrix used for UI (orthographic duh)
Matrix4 uiProjectionMatrix;

//The skybox class is just a cubemap wrapper right now...
Skybox sky;

//This is the context OpenGL will use to render stuff
GLContext glContext;

//Shaders
GLProgram program_Lit;
GLProgram program_Unlit;
GLProgram program_Sky;

//This is my cool theoretically cross-platform window class
Window window;

bool cursorLocked = false;

//Set when cursor is locked to window
POINT lockPos;

volatile float axisX = 0.f, axisY = 0.f, axisZ = 0.f;
volatile float lookX = 0.f, lookY = 0.f, lookZ = 0.f;
volatile float mouseLookX = 0.f, mouseLookY = 0.f;

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

//Here's the entry point
int Main()
{
	//I have to create a stupid context to load some extensions (but not all)
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

	//No vsync
	wglSwapIntervalEXT(0);

	//Backface culling
	glEnable(GL_CULL_FACE);

	//Depth testing
	glEnable(GL_DEPTH_TEST);


	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	
	//Traslucency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Winding order of front face
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
	//This will allocate and initialise all of the asset managers
	engine.Init(EEngineCreateFlags::ALL);

	//Add Windows/Fonts directory so I can load some TTF fonts from it
	engine.pFontManager->AddPath(Utilities::GetSystemFontDir());

	ModelManager& modelManager = *engine.pModelManager;
	TextureManager& textureManager = *engine.pTextureManager;
	InputManager& inputManager = *engine.pInputManager;

	engine.pMaterialManager->SetRootPath("Data/");
	engine.pMeshManager->SetRootPath("Data/");
	modelManager.SetRootPath("Data/");
	textureManager.SetRootPath("Data/");
	engine.pFontManager->SetRootPath("Data/");

	inputManager.BindKeyAxis(EKeycode::A, (float*)&axisX, -1);
	inputManager.BindKeyAxis(EKeycode::D, (float*)&axisX, 1);
	inputManager.BindKeyAxis(EKeycode::F, (float*)&axisY, -1);
	inputManager.BindKeyAxis(EKeycode::LSHIFT, (float*)&axisY, -1);
	inputManager.BindKeyAxis(EKeycode::SPACE, (float*)&axisY, 1);
	inputManager.BindKeyAxis(EKeycode::S, (float*)&axisZ, -1);
	inputManager.BindKeyAxis(EKeycode::W, (float*)&axisZ, 1);

	inputManager.BindKeyAxis(EKeycode::DOWN, (float*)&lookX, -1);
	inputManager.BindKeyAxis(EKeycode::UP, (float*)&lookX, 1);
	inputManager.BindKeyAxis(EKeycode::LEFT, (float*)&lookY, -1);
	inputManager.BindKeyAxis(EKeycode::RIGHT, (float*)&lookY, 1);
	inputManager.BindKeyAxis(EKeycode::Q, (float*)&lookZ, -1);
	inputManager.BindKeyAxis(EKeycode::E, (float*)&lookZ, 1);

	inputManager.BindAxis(EInputAxis::MOUSE_X, (float*)&mouseLookY);
	inputManager.BindAxis(EInputAxis::MOUSE_Y, (float*)&mouseLookX);


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

	camera = world.CreateObject<OCamera>();
	if (ORTHO)
	{
		camera->GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
		camera->GetProjection().SetNearFar(-100000.f, 100000.f);
		camera->GetProjection().SetOrthographicScale(128.f); //pixels per unit
		camera->SetRelativeRotation(Vector3(-90.f, 0.f, 0.f));
	}
	else
	{
		camera->GetProjection().SetType(EProjectionType::PERSPECTIVE);
		camera->GetProjection().SetNearFar(0.001f, 100000.f);
		camera->SetRelativePosition(Vector3(0.f, 0.f, -5.f));
	}

	light1 = world.CreateObject<OLight>();
	light2 = world.CreateObject<OLight>();
	light3 = world.CreateObject<OLight>();
	light4 = world.CreateObject<OLight>();

	light1->SetRelativePosition(Vector3(0.f, 0.f, -4.f));
	light2->SetRelativePosition(Vector3(0.f, 0.f, -3.5f));
	light3->SetRelativePosition(Vector3(0.f, 0.f, -3.f));
	light4->SetRelativePosition(Vector3(0.f, 0.f, -2.f));

	light1->SetColour(Vector3(1.f, 0.f, 0.f));
	light2->SetColour(Vector3(0.f, 1.f, 0.f));
	light3->SetColour(Vector3(0.f, 0.f, 1.f));

	light1->SetRadius(5.f);
	light2->SetRadius(5.f);
	light3->SetRadius(5.f);
	light4->SetRadius(10.f);

	cubeParent = world.CreateObject<WorldObject>();
	lightParent1 = world.CreateObject<WorldObject>();
	lightParent2 = world.CreateObject<WorldObject>();
	lightParent3 = world.CreateObject<WorldObject>();
	lightParent4 = world.CreateObject<WorldObject>();

	light1->SetParent(lightParent1, false);
	light2->SetParent(lightParent2, false);
	light3->SetParent(lightParent3, false);
	light4->SetParent(lightParent4, false);

	cube = world.CreateObject<ORenderable>();
	lightParent4->SetParent(cube, false);

	cube->SetRelativePosition(Vector3(2.f, 0.f, 0.f));
	cube->SetParent(cubeParent, false);
	cube->SetModel(modelManager.Cube(), engine.context);
	cube->SetMaterial("bricks", engine.context);

	SharedPointer<MaterialSurface> shiny = SharedPointer<MaterialSurface>::Create(new MaterialSurface);
	shiny->SetDiffuse(textureManager.White());
	shiny->SetSpecular(textureManager.White());
	shiny->SetReflection(textureManager.White());

	renderable = world.CreateObject<ORenderable>();
	renderable->SetParent(cubeParent, false);
	renderable->SetModel(meshName, engine.context);
	renderable->SetMaterial(shiny.Cast<const Material>());

	window.SetTitle("Window");

	//
	//Event loop
	//
	running = true;
	WindowEvent e;
	while (running)
	{
		timer.Start();
		t += dt;

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

				camera->GetProjection().SetDimensions(Vector2T(w, h));
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
		dt = timer.SecondsSinceStart();
	}

	return 0;

}

void Frame()
{
	engine.pInputManager->ClearMouseInput();
	if (cursorLocked) {
		POINT cursorPos;
		::GetCursorPos(&cursorPos);
		engine.pInputManager->AddMouseInput((float)(cursorPos.x - lockPos.x), -(float)(cursorPos.y - lockPos.y));
		::SetCursorPos(lockPos.x, lockPos.y);
	}

	float moveAmount = MOVERATE * dt;
	float turnAmount = TURNRATE * dt;

	camera->MoveAligned(Vector3(axisX * moveAmount, axisY * moveAmount, axisZ * moveAmount));
	camera->AddRelativeRotation(Vector3(lookX * turnAmount + mouseLookX * MOUSESENS, lookY * turnAmount + mouseLookY * MOUSESENS, lookZ * turnAmount));

	lightParent1->RotateRelative(rotationOffset1 * dt);
	lightParent2->RotateRelative(rotationOffset2 * dt);
	lightParent3->RotateRelative(rotationOffset3 * dt);
	lightParent4->RotateRelative(rotationOffset4 * dt);

	cubeParent->RotateRelative(cubeParentRotationOffset * dt);
	cube->RotateRelative(cubeRotationOffset * dt);

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
		renderQueue.Clear();
		uiQueue.Clear();

		camera->Use(renderQueue, ERenderChannels::SURFACE | ERenderChannels::UNLIT); //Sky uses custom matrices
		world.Render(renderQueue, nullptr);
		sky.Render(renderQueue);
		
		//grid
		float limit = camera->GetProjection().GetType() == EProjectionType::ORTHOGRAPHIC ? 1.f : 10.f;
		RenderEntry& e = renderQueue.CreateEntry(ERenderChannels::UNLIT);
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
		e.AddSetColour(Colour::Red);
		e.AddGrid(camera->GetAbsoluteTransform(), camera->GetProjection(), EAxis::X, 1.f, limit, 0.f, 0.f);
		e.AddSetColour(Colour::Green);
		e.AddGrid(camera->GetAbsoluteTransform(), camera->GetProjection(), EAxis::Y, 1.f, limit, 0.f, 0.f);
		e.AddSetColour(Colour::Blue);
		e.AddGrid(camera->GetAbsoluteTransform(), camera->GetProjection(), EAxis::Z, 1.f, limit, 0.f, 0.f);

		//UI
		//fps string
		static Transform fontTransform(Vector3(0, 0, 0), Rotation(), Vector3(64, 0, 0));
		RenderEntry& strEntry = uiQueue.CreateEntry(ERenderChannels::UNLIT);
		strEntry.AddSetColour(Colour::Yellow);
		engine.pFontManager->Get(fontName, engine.context)->RenderString(strEntry, fpsString.begin(), fontTransform);

		//Begin drawing
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//Draw unlit
		{
			//Bind the unlit shader to the current GL context
			program_Unlit.Use();

			//This renders all the stuff in the queue
			//This will only render entries in the queue that contain the channels UNLIT or EDITOR
			renderQueue.Render(ERenderChannels::UNLIT | ERenderChannels::EDITOR, engine.pMeshManager, engine.pTextureManager, 0);
		}

		{
			program_Lit.Use();

			//The _engine is a tad pooey in the aspect that the texture units 0, 1, 2, and 3 are always bound to the diffuse, normal, specular, and reflectivity textures respectively.
			//No PBR yet!
			program_Lit.SetInt(DefaultUniformVars::intCubemap, 100);
			program_Lit.SetInt(DefaultUniformVars::intTextureDiffuse, 0);
			program_Lit.SetInt(DefaultUniformVars::intTextureNormal, 1);
			program_Lit.SetInt(DefaultUniformVars::intTextureSpecular, 2);
			program_Lit.SetInt(DefaultUniformVars::intTextureReflection, 3);

			renderQueue.Render(ERenderChannels::SURFACE, engine.pMeshManager, engine.pTextureManager, program_Lit.GetInt(DefaultUniformVars::intLightCount));
		}

		//Draw sky
		if (camera->GetProjection().GetType() == EProjectionType::PERSPECTIVE)
		{
			program_Sky.Use();

			//bind the view matrix to this instead of the default thing......
			Matrix4 skyView = camera->GetAbsoluteTransform().GetInverseMatrix();

			//....because I strip out the translation from the matrix
			skyView[3][0] = skyView[3][1] = skyView[3][2] = 0;

			program_Sky.SetMatrix4(DefaultUniformVars::mat4Projection, camera->GetProjection().GetMatrix());
			program_Sky.SetMatrix4(DefaultUniformVars::mat4View, skyView);

			glDepthFunc(GL_LEQUAL);
			renderQueue.Render(ERenderChannels::SKY, engine.pMeshManager, engine.pTextureManager, 0);
			glDepthFunc(GL_LESS);
		}

		//Draw UI
		{
			program_Unlit.Use();
			program_Unlit.SetMatrix4(DefaultUniformVars::mat4Projection, uiProjectionMatrix);
			program_Unlit.SetMatrix4(DefaultUniformVars::mat4View, Matrix4::Identity());
			uiQueue.Render(ERenderChannels::UNLIT, engine.pMeshManager, engine.pTextureManager, 0);
		}
			
		//
		window.SwapBuffers();
	}
}
