#include "ResourceSelect.hpp"
#include <CommCtrl.h>
#include <Engine/Colour.hpp>
#include <Engine/Debug.hpp>
#include <Engine/DrawUtils.hpp>
#include <Engine/EntCamera.hpp>
#include <Engine/EntLight.hpp>
#include <Engine/EntRenderable.hpp>
#include <Engine/EntSprite.hpp>
#include <Engine/GL.hpp>
#include <Engine/GLContext.hpp>
#include <Engine/GLProgram.hpp>
#include <Engine/IO.hpp>
#include <Engine/MaterialManager.hpp>
#include <Engine/ModelManager.hpp>
#include <Engine/TextureManager.hpp>
#include <Engine/Utilities.hpp>
#include <Engine/Window.hpp>
#include "resource.h"

constexpr LPCTSTR viewportClassName = TEXT("RESSELECTCLASS");

class RSDialog
{
private:
	const GLContext* glContext;
	GLProgram* programLit;
	const GLProgram* programUnlit;
	MaterialManager* const materialManager;
	ModelManager* const modelManager;

	Entity _cameraRoot;
	EntCamera _camera;
	EntRenderable _object;
	EntSprite _sprite;
	EntLight _light;

	bool _moving;
	bool _sizing;

public:
	String currentName = "";

	union
	{
		SharedPointer<const Model> selectionAsModel;
		SharedPointer<const Material> selectionAsMaterial;
		SharedPointer<const Asset> selection;
	};

	bool isModelSelect = false;

	HDC viewportDC;

	HIMAGELIST imageList = NULL;

	RSDialog(const GLContext& context, GLProgram& programLit, const GLProgram& programUnlit, MaterialManager& materialManager, ModelManager& modelManager) :
		_moving(false),
		_sizing(false),
		glContext(&context),
		programLit(&programLit),
		programUnlit(&programUnlit),
		materialManager(&materialManager),
		modelManager(&modelManager),
		selection(),
		viewportDC(NULL)
	{}

	~RSDialog() {}

	inline const String &GetRootPath() { return isModelSelect ? modelManager->GetRootPath() : materialManager->GetRootPath(); }

	void InitScene(int vpW, int vpH)
	{
		//opengl
		WindowFunctions::SetDefaultPixelFormat(viewportDC);
		glContext->Use(viewportDC);

		_camera.SetViewport(vpW, vpH);

		_camera.SetParent(&_cameraRoot);

		_camera.SetProjectionType(ProjectionType::PERSPECTIVE);
		_camera.SetFOV(90.f);
		_camera.SetRelativePosition(Vector3(0.f, 0.f, 1.f));
		_camera.SetRelativeRotation(Vector3(0.f, 180.f, 0.f));

		_light.SetParent(&_camera);
		_light.SetRelativePosition(Vector3(0.5f, 0.5f, 0.f));

		if (!isModelSelect)
			_object.SetModel(modelManager->Cube());

		_moving = false;
		_sizing = false;
	}

	void Update()
	{
		if (isModelSelect)
			_object.SetModel(selectionAsModel);
		else
		{
			if (dynamic_cast<const MaterialSprite*>(selectionAsMaterial.Ptr()))
			{
				_sprite.SetMaterial(selectionAsMaterial.Cast<const MaterialSprite>());
				_object.SetMaterial(SharedPointer<const Material>());
			}
			else
			{
				_sprite.SetMaterial(SharedPointer<const MaterialSprite>());
				_object.SetMaterial(selectionAsMaterial);
			}
		}
	}

	void Draw()
	{
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

			_light.ToShader(0);

			_object.Render(RenderChannel::SURFACE);
		}

		programUnlit->Use();
		_camera.Use();

		Engine::Instance().pTextureManager->White()->Bind(0);

		programUnlit->SetVec4(DefaultUniformVars::vec4Colour, Colour::Red);
		DrawUtils::DrawLine(*Engine::Instance().pModelManager, Vector3(-1.f, 0.f, 0.f), Vector3(1.f, 0.f, 0.f));

		programUnlit->SetVec4(DefaultUniformVars::vec4Colour, Colour::Green);
		DrawUtils::DrawLine(*Engine::Instance().pModelManager, Vector3(0.f, -1.f, 0.f), Vector3(0.f, 1.f, 0.f));

		programUnlit->SetVec4(DefaultUniformVars::vec4Colour, Colour::Blue);
		DrawUtils::DrawLine(*Engine::Instance().pModelManager, Vector3(0.f, 0.f, -1.f), Vector3(0.f, 0.f, 1.f));

		programUnlit->SetVec4(DefaultUniformVars::vec4Colour, Colour::White);
		_sprite.Render(RenderChannel::SPRITE);

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
			keynames = Engine::Instance().pModelManager->GetAllPossibleKeys();
		else
			keynames = Engine::Instance().pMaterialManager->GetAllPossibleKeys();

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
				rs->selection = Engine::Instance().pModelManager->Get(rs->currentName).Cast<const Asset>();
			else
				rs->selection = Engine::Instance().pMaterialManager->Get(rs->currentName).Cast<const Asset>();

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
			::SetCursor(NULL);

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

String ResourceSelect::Dialog(MaterialManager &materialManager, ModelManager &modelManager, const char *search, HWND parent, ResourceType type, const GLContext &context, GLProgram &programLit, const GLProgram &programUnlit)
{
	RSDialog rs(context, programLit, programUnlit, materialManager, modelManager);
	rs.isModelSelect = type == ResourceType::MODEL;

	if (::DialogBoxParam(NULL, MAKEINTRESOURCE(IDD_RES_SELECT), parent, (DLGPROC)RSDialog::DialogProc, (LPARAM)&rs) == 1)
		return rs.currentName;

	return "";
}
