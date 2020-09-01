#include "ResourceSelect.hpp"
#include <CommCtrl.h>
#include <Engine/EngineInstance.hpp>
#include <Engine/EntCamera.hpp>
#include <Engine/EntLight.hpp>
#include <Engine/EntRenderable.hpp>
#include <Engine/EntSprite.hpp>
#include <Engine/ModelManager.hpp>
#include <ELGraphics/Colour.hpp>
#include <ELGraphics/IO.hpp>
#include <ELGraphics/MaterialManager.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELGraphics/TextureManager.hpp>
#include <ELSys/Debug.hpp>
#include <ELSys/GL.hpp>
#include <ELSys/GLContext.hpp>
#include <ELSys/GLProgram.hpp>
#include <ELSys/System.hpp>
#include <ELSys/Utilities.hpp>
#include <ELSys/Window.hpp>
#include "resource.h"

constexpr LPCTSTR viewportClassName = TEXT("RESSELECTCLASS");

class RSDialog
{
private:
	EngineInstance& engine;

	const GLContext* glContext;
	GLProgram* programLit;
	const GLProgram* programUnlit;

	RenderQueue renderQueue;

	Entity _cameraRoot;
	EntCamera _camera;
	EntRenderable _object;
	EntSprite _sprite;
	EntLight _light;

	bool _moving;
	bool _sizing;

public:
	String currentName = "";

	SharedPointer<const Asset> selection;

	bool isModelSelect = false;

	HDC viewportDC;

	HIMAGELIST imageList = NULL;

	RSDialog(EngineInstance& engine, const GLContext& context, GLProgram& programLit, const GLProgram& programUnlit) :
		engine(engine),
		_moving(false),
		_sizing(false),
		glContext(&context),
		programLit(&programLit),
		programUnlit(&programUnlit),
		selection(),
		viewportDC(NULL)
	{}

	~RSDialog() {}

	inline const String &GetRootPath() { return isModelSelect ? engine.pModelManager->GetRootPath() : engine.pMaterialManager->GetRootPath(); }

	void InitScene(int vpW, int vpH)
	{
		//opengl
		WindowFunctions::SetDefaultPixelFormat(viewportDC);
		glContext->Use_Win32(viewportDC);

		_camera.GetProjection().SetDimensions(Vector2T(vpW, vpH));

		_camera.SetParent(&_cameraRoot);

		_camera.GetProjection().SetType(EProjectionType::PERSPECTIVE);
		_camera.GetProjection().SetPerspectiveFOV(90.f);
		_camera.SetRelativePosition(Vector3(0.f, 0.f, 1.f));
		_camera.SetRelativeRotation(Vector3(0.f, 180.f, 0.f));

		_light.SetParent(&_camera);
		_light.SetRelativePosition(Vector3(0.5f, 0.5f, 0.f));

		if (!isModelSelect)
			_object.SetModel(engine.pModelManager->Cube(), engine.context);

		_moving = false;
		_sizing = false;
	}

	void Update()
	{
		const Model* pModel = dynamic_cast<const Model*>(selection.Ptr());

		if (pModel)
		{
			SharedPointer<const Model> model = engine.pModelManager->Find(pModel);

			if (model)
			{
				_object.SetModel(model, engine.context);
				
				if (!_object.GetMaterial())
					_object.SetMaterial(engine.pMaterialManager->Get("white", engine.context));
			}
			else
				Debug::Error("Modelmanager cannot track a model ptr that doesn't belong to it!");
		}
		else
		{
			const Material* pMaterial = dynamic_cast<const Material*>(selection.Ptr());

			if (pMaterial)
			{
				SharedPointer<const Material> material = engine.pMaterialManager->Find(pMaterial);
				const MaterialSprite* pSprite = dynamic_cast<const MaterialSprite*>(pMaterial);

				if (!material)
				{
					Debug::Error("Materialmgr cannot track a material ptr that doesn't belong to it!");
					return;
				}

				if (pSprite)
				{
					_object.SetMaterial(SharedPointer<const Material>());
					_sprite.SetMaterial(material.Cast<const MaterialSprite>());
				}
				else
				{
					_sprite.SetMaterial(SharedPointer<const MaterialSprite>());
					_object.SetMaterial(material);
				}
			}
		}
	}

	void Draw()
	{
		renderQueue.ClearDynamicQueue();
		_light.Render(renderQueue);
		_object.Render(renderQueue);
		_sprite.Render(renderQueue);

		RenderEntry& e = renderQueue.NewDynamicEntry(ERenderChannels::UNLIT);
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
		e.AddSetColour(Colour::Red);
		e.AddLine(Vector3(-1.f, 0.f, 0.f), Vector3(1.f, 0.f, 0.f));

		e.AddSetColour(Colour::Green);
		e.AddLine(Vector3(0.f, -1.f, 0.f), Vector3(0.f, 1.f, 0.f));

		e.AddSetColour(Colour::Blue);
		e.AddLine(Vector3(0.f, 0.f, -1.f), Vector3(0.f, 0.f, 1.f));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDepthFunc(GL_LESS);


		if (_object.GetMaterial())
		{
			programLit->Use();
			_camera.Use();
			programLit->SetVec4(DefaultUniformVars::vec4Colour, Colour::White);

			programLit->SetInt(DefaultUniformVars::intTextureDiffuse, 0);
			programLit->SetInt(DefaultUniformVars::intTextureNormal, 1);
			programLit->SetInt(DefaultUniformVars::intTextureSpecular, 2);
			programLit->SetInt(DefaultUniformVars::intTextureReflection, 3);

			programLit->SetVec2(DefaultUniformVars::vec2UVOffset, Vector2());
			programLit->SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1, 1));

			renderQueue.Render(ERenderChannels::SURFACE, *engine.pMeshManager, *engine.pTextureManager, programLit->GetInt(DefaultUniformVars::intLightCount));
		}

		programUnlit->Use();
		_camera.Use();
		renderQueue.Render(ERenderChannels::UNLIT | ERenderChannels::SPRITE, *engine.pMeshManager, *engine.pTextureManager, 0);

		::SwapBuffers(viewportDC);
	};

	static INT_PTR DialogProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT ViewportProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

//static
INT_PTR RSDialog::DialogProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RSDialog *rs = (RSDialog*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_INITDIALOG:
	{
		rs = (RSDialog*)lparam;
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)rs);

		HINSTANCE instance = ::GetModuleHandle(NULL);

		//ImageList
		const int imageCount = 2;

		rs->imageList = ::ImageList_Create(16, 16, FALSE, imageCount, 0);

		HBITMAP bmp = ::LoadBitmap(instance, MAKEINTRESOURCE(IDB_TREENODE));
		::ImageList_Add(rs->imageList, bmp, NULL);
		::DeleteObject(bmp);

		bmp = ::LoadBitmap(instance, MAKEINTRESOURCE(IDB_TREENODE2));
		::ImageList_Add(rs->imageList, bmp, NULL);
		::DeleteObject(bmp);

		if (::ImageList_GetImageCount(rs->imageList) < imageCount)
			Debug::Error("Oh dear. Somehow the tree view's imagelist could not be created properly.");

		::SendDlgItemMessage(hwnd, IDC_TREE, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, (LPARAM)rs->imageList);

		TVINSERTSTRUCT insertStruct = {};
		insertStruct.hParent = NULL;
		insertStruct.hInsertAfter = TVI_LAST;
		insertStruct.item.iImage = 0;
		insertStruct.item.iSelectedImage = 1;
		insertStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

		Buffer<String> keynames;

		if (rs->isModelSelect)
			keynames = rs->engine.pModelManager->GetAllPossibleKeys();
		else
			keynames = rs->engine.pMaterialManager->GetAllPossibleKeys();

		for (uint32 i = 0; i < keynames.GetSize(); ++i)
		{
			insertStruct.item.lParam = (LPARAM)i;
			insertStruct.item.pszText = &keynames[i][0];
			insertStruct.item.cchTextMax = (int)keynames[i].GetLength();
			::SendDlgItemMessage(hwnd, IDC_TREE, TVM_INSERTITEM, 0, (LPARAM)&insertStruct);
		}

		HWND vp = ::GetDlgItem(hwnd, IDC_VIEWPORT);

		RECT rect;
		::GetClientRect(vp, &rect);

		HWND viewport = ::CreateWindow(viewportClassName, NULL, WS_CHILD | WS_BORDER | WS_VISIBLE, 0, 0, rect.right, rect.bottom, vp, NULL, instance, NULL);
		rs->viewportDC = ::GetDC(viewport);
		rs->InitScene(rect.right, rect.bottom);
		::SetWindowLongPtr(viewport, GWLP_USERDATA, (LONG_PTR)rs);
	}

	break;
	case WM_CLOSE:
		::EndDialog(hwnd, 0);
		break;

	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case IDOK:
			::EndDialog(hwnd, 1);
			break;
		}
		break;

	case WM_DESTROY:
		::ImageList_Destroy(rs->imageList);
		break;

	case WM_NOTIFY:
	{
		LPNMHDR notify = (LPNMHDR)lparam;

#pragma warning(suppress: 26454)
		if (notify->code == TVN_SELCHANGED && notify->idFrom == IDC_TREE)
		{
			constexpr int bufSize = 256;
			char textBuffer[bufSize];

			LPNMTREEVIEW nmtv = (LPNMTREEVIEW)lparam;
			nmtv->itemNew.mask = TVIF_TEXT;
			nmtv->itemNew.pszText = textBuffer;
			nmtv->itemNew.cchTextMax = bufSize;
			auto result = ::SendDlgItemMessageA(hwnd, IDC_TREE, TVM_GETITEM, 0, (LPARAM)&nmtv->itemNew);
			
			rs->currentName = textBuffer;

			if (rs->isModelSelect)
				rs->selection = rs->engine.pModelManager->Get(rs->currentName, rs->engine.context).Cast<const Asset>();
			else
				rs->selection = rs->engine.pMaterialManager->Get(rs->currentName, rs->engine.context).Cast<const Asset>();

			rs->Update();
			rs->Draw();

			String fullpath = rs->GetRootPath() + rs->currentName + ".txt";

			::SendDlgItemMessage(hwnd, IDC_NAME, WM_SETTEXT, 0, (LPARAM)fullpath.GetData());

			::SendDlgItemMessage(hwnd, IDC_INFO, WM_SETTEXT, 0, 
				(LPARAM)Utilities::WithCarriageReturns(IO::ReadFileString(fullpath.GetData(), true)).GetData());
		}
	}
	break;

	default:
		return FALSE;
	}

	return TRUE;
}

LRESULT RSDialog::ViewportProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RSDialog *rs = (RSDialog*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_LBUTTONDOWN:
		rs->_moving = true;
		break;
	case WM_RBUTTONDOWN:
		rs->_sizing = true;
		break;
	case WM_LBUTTONUP:
		rs->_moving = false;
		break;
	case WM_RBUTTONUP:
		rs->_sizing = false;
		break;

	case WM_MOUSEMOVE:
		if (rs->_moving || rs->_sizing)
		{
			System::SetCursor(ECursor::NONE);

			RECT rect;
			::GetWindowRect(hwnd, &rect);
			int midPointX = (rect.left + rect.right) / 2;
			int midPointY = (rect.top + rect.bottom) / 2;

			POINT cursor;
			::GetCursorPos(&cursor);

			int deltaX = cursor.x - midPointX;
			int deltaY = cursor.y - midPointY;

			::SetCursorPos(midPointX, midPointY);

			if (rs->_moving)
			{
				rs->_cameraRoot.AddRelativeRotation(Vector3((float)deltaY, (float)deltaX, 0));
			}
			else
			{
				rs->_camera.RelativeMove(Vector3(0.f, 0.f, deltaY / 50.f));
			}

			rs->Draw();
		}
	break;


	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

void ResourceSelect::Initialise()
{
	WNDCLASSEXA windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.lpfnWndProc = (WNDPROC)RSDialog::ViewportProc;
	windowClass.hInstance = ::GetModuleHandle(NULL);
	windowClass.hbrBackground = (HBRUSH)(COLOR_ACTIVECAPTION + 1);
	windowClass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = viewportClassName;
	::RegisterClassEx(&windowClass);
}

String ResourceSelect::Dialog(EngineInstance& engine, const char *search, HWND parent, EResourceType type, const GLContext &context, GLProgram &programLit, const GLProgram &programUnlit)
{
	RSDialog rs(engine, context, programLit, programUnlit);
	rs.isModelSelect = type == EResourceType::MODEL;

	if (::DialogBoxParam(NULL, MAKEINTRESOURCE(IDD_RES_SELECT), parent, (DLGPROC)RSDialog::DialogProc, (LPARAM)&rs) == 1)
		return rs.currentName;

	return "";
}
