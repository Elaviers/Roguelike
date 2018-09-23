#include "ResourceSelect.h"
#include <CommCtrl.h>
#include <Engine/Camera.h>
#include <Engine/Error.h>
#include <Engine/GL.h>
#include <Engine/GLContext.h>
#include <Engine/GLProgram.h>
#include <Engine/IO.h>
#include <Engine/Light.h>
#include <Engine/MaterialManager.h>
#include <Engine/ModelManager.h>
#include <Engine/Renderable.h>
#include <Engine/Utilities.h>
#include <Engine/Window.h>
#include "resource.h"

constexpr LPCTSTR viewportClassName = TEXT("RESSELECTVPCLASS");

class RSDialog;

struct VPInfo
{
	Transform *transform;
	bool moving;
	bool sizing;

	const RSDialog *parent;
};

class RSDialog
{
private:
	const GLContext *glContext;
	const GLProgram *glProgram;
	MaterialManager* const materialManager;
	ModelManager* const modelManager;

	Camera _camera;
	Renderable _object;
	Light _light;
public:
	Buffer<String> paths;
	int selection;

	bool isModelSelect;

	HDC viewportDC;
	VPInfo viewportInfo;

	HIMAGELIST imageList;

	RSDialog(const GLContext &context, const GLProgram &program, MaterialManager& materialManager, ModelManager &modelManager) : 
		glContext(&context), glProgram(&program), materialManager(&materialManager), modelManager(&modelManager) {}

	inline const String &GetRootPath() { return isModelSelect ? modelManager->GetRootPath() : materialManager->GetRootPath(); }

	void InitScene(int vpW, int vpH)
	{
		//opengl
		WindowFunctions::SetDefaultPixelFormat(viewportDC);
		glContext->Use(viewportDC);

		glViewport(0, 0, vpW, vpH);
		_camera.SetViewport(vpW, vpH);

		_camera.SetProectionType(ProjectionType::PERSPECTIVE);
		_camera.SetFOV(90.f);
		_camera.transform.SetPosition(Vector3(-.7f, .7f, -.7f));
		_camera.transform.SetRotation(Vector3(-45.f, 45.f, 0.f));

		_light.transform.SetPosition(Vector3(-1.f, .5f, -2.f));

		if (!isModelSelect)
			_object.SetModel(&modelManager->Cube());

		viewportInfo.parent = this;
		viewportInfo.transform = &_object.transform;
		viewportInfo.moving = false;
		viewportInfo.sizing = false;
	}

	void Update()
	{
		if (isModelSelect)
			_object.SetModel(modelManager->GetModel(paths[selection]));
		else
			_object.SetMaterial(materialManager->GetMaterial(paths[selection]));
	}

	void Draw() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glProgram->Use();
		glProgram->SetMat4(DefaultUniformVars::mat4Projection, _camera.GetProjectionMatrix());
		glProgram->SetMat4(DefaultUniformVars::mat4View, _camera.transform.MakeInverseTransformationMatrix());
		glProgram->SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 1.f, 1.f, 1.f));

		glProgram->SetInt(DefaultUniformVars::intTextureDiffuse, 0);
		glProgram->SetInt(DefaultUniformVars::intTextureNormal, 1);
		glProgram->SetInt(DefaultUniformVars::intTextureSpecular, 2);
		glProgram->SetInt(DefaultUniformVars::intTextureReflection, 3);

		_light.ToShader(0);

		_object.Render();



		::SwapBuffers(viewportDC);
	};
};

//static
INT_PTR DialogProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
			Error("Oh dear. Somehow the tree view's imagelist could not be created properly.");

		::SendDlgItemMessage(hwnd, IDC_TREE, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, (LPARAM)rs->imageList);

		TVINSERTSTRUCT insertStruct = {};
		insertStruct.hParent = NULL;
		insertStruct.hInsertAfter = TVI_LAST;
		insertStruct.item.iImage = 0;
		insertStruct.item.iSelectedImage = 1;
		insertStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

		for (uint32 i = 0; i < rs->paths.GetSize(); ++i)
		{
			insertStruct.item.lParam = (LPARAM)i;
			insertStruct.item.pszText = &rs->paths[i][0];
			insertStruct.item.cchTextMax = rs->paths[i].GetLength();
			::SendDlgItemMessage(hwnd, IDC_TREE, TVM_INSERTITEM, 0, (LPARAM)&insertStruct);
		}

		HWND vp = ::GetDlgItem(hwnd, IDC_VIEWPORT);

		RECT rect;
		::GetClientRect(vp, &rect);

		HWND viewport = ::CreateWindow(viewportClassName, NULL, WS_CHILD | WS_BORDER | WS_VISIBLE, 0, 0, rect.right, rect.bottom, vp, NULL, instance, NULL);
		rs->viewportDC = ::GetDC(viewport);
		rs->InitScene(rect.right, rect.bottom);
		::SetWindowLongPtr(viewport, GWLP_USERDATA, (LONG_PTR)&rs->viewportInfo);
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

		if (notify->code == TVN_SELCHANGED && notify->idFrom == IDC_TREE)
		{
			LPNMTREEVIEW nmtv = (LPNMTREEVIEW)lparam;
			rs->selection = (int)nmtv->itemNew.lParam;
			rs->Update();
			rs->Draw();

			String fullpath = rs->GetRootPath() + rs->paths[rs->selection] + ".txt";

			::SendDlgItemMessage(hwnd, IDC_NAME, WM_SETTEXT, 0, (LPARAM)fullpath.GetData());
			::SendDlgItemMessage(hwnd, IDC_INFO, WM_SETTEXT, 0, (LPARAM)IO::ReadFileString(fullpath.GetData()).GetData());
		}
	}
	break;

	default:
		return FALSE;
	}

	return TRUE;
}

LPARAM viewportProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	VPInfo *vpInfo = (VPInfo*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_LBUTTONDOWN:
		vpInfo->moving = true;
		break;
	case WM_RBUTTONDOWN:
		vpInfo->sizing = true;
		break;
	case WM_LBUTTONUP:
		vpInfo->moving = false;
		break;
	case WM_RBUTTONUP:
		vpInfo->sizing = false;
		break;

	case WM_MOUSEMOVE:
		if (vpInfo->moving || vpInfo->sizing)
		{
			RECT rect;
			::GetWindowRect(hwnd, &rect);
			int midPointX = (rect.left + rect.right) / 2;
			int midPointY = (rect.top + rect.bottom) / 2;

			POINT cursor;
			::GetCursorPos(&cursor);

			int deltaX = cursor.x - midPointX;
			int deltaY = cursor.y - midPointY;

			::SetCursorPos(midPointX, midPointY);

			if (vpInfo->moving)
			{
				vpInfo->transform->Rotate(Vector3((float)-deltaY, (float)-deltaX, 0));
			}
			else
			{
				float scaling = deltaY / -50.f;
				Vector3 currentScale = vpInfo->transform->GetScale();
				vpInfo->transform->SetScale(Vector3(currentScale[0] + scaling, currentScale[1] + scaling, currentScale[2] + scaling));
			}

			vpInfo->parent->Draw();
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
	windowClass.lpfnWndProc = viewportProc;
	windowClass.hInstance = ::GetModuleHandle(NULL);
	windowClass.hbrBackground = (HBRUSH)(COLOR_ACTIVECAPTION + 1);
	windowClass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = viewportClassName;
	::RegisterClassEx(&windowClass);
}

String ResourceSelect::Dialog(MaterialManager &materialManager, ModelManager &modelManager, const char *search, HWND parent, ResourceType type, const GLContext &context, const GLProgram &program)
{
	RSDialog rs(context, program, materialManager, modelManager);
	rs.isModelSelect = type == ResourceType::MODEL;
	rs.paths = IO::FindFilesInDirectory(search);

	for (uint32 i = 0; i < rs.paths.GetSize(); ++i)
		Utilities::StripExtension(rs.paths[i]);

	if (::DialogBoxParam(NULL, MAKEINTRESOURCE(IDD_RES_SELECT), parent, DialogProc, (LPARAM)&rs) == 1)
		return rs.paths[rs.selection];

	return "";
}
