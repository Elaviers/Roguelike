#include "PropertyWindow.h"
#include <CommCtrl.h>
#include <Engine/GameObject.h>
#include "Editor.h"

constexpr int buttonW = 20;
constexpr int boxH = 18;

LPCTSTR PropertyWindow::_className = TEXT("PWCLASS");
WNDPROC PropertyWindow::_defaultEditProc;

//static
LRESULT PropertyWindow::_WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PropertyWindow* pw = (PropertyWindow*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT create = (LPCREATESTRUCT)lparam;
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)create->lpCreateParams);
	}
	break;

	case WM_CLOSE:
		::ShowWindow(hwnd, SW_HIDE);
		break;

	case WM_SIZE:
	{
		SHORT w = LOWORD(lparam);

		uint16 label_w = w / 2;
		uint16 box_w = w / 2;
		uint16 y = 0;

		for (uint32 i = 0; i < pw->_child_hwnds.GetSize(); ++i)
		{
			WindowFunctions::SetHWNDSizeAndPos(pw->_child_hwnds[i].label, 0, y, label_w, boxH);
			WindowFunctions::SetHWNDSizeAndPos(pw->_child_hwnds[i].box, label_w, y, box_w - (pw->_child_hwnds[i].button ? buttonW : 0), boxH);
			WindowFunctions::RepositionHWND(pw->_child_hwnds[i].button, label_w + box_w - buttonW, y);

			y += boxH;
		}
	}
		break;

	case WM_COMMAND:
	{
		if (HIWORD(wparam) == EN_KILLFOCUS)
		{
			char string[32];
			::GetWindowText((HWND)lparam, string, 32);

			pw->_child_hwnds[LOWORD(wparam)].property->SetByString(String(string));
		}
		else if (HIWORD(wparam) == BN_CLICKED)
		{
			uint32 id = LOWORD(wparam);

			String string;

			if (pw->_child_hwnds[id].property->GetFlags() & PropertyFlags::MATERIAL)
				string = pw->_owner->SelectMaterialDialog();
			else if (pw->_child_hwnds[id].property->GetFlags() & PropertyFlags::MODEL)
				string = pw->_owner->SelectModelDialog();
			else break;

			if (string.GetLength() > 0)
			{
				::SetWindowTextA(pw->_child_hwnds[id].box, string.GetData());
				pw->_child_hwnds[id].property->SetByString(string);
			}
		}
	}
		break;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

//static
LRESULT PropertyWindow::_EditProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wparam == VK_RETURN)
		{
			if (lparam & (1 << 30))
				break;

			PropertyWindow *pw = (PropertyWindow*)::GetWindowLongPtr(::GetParent(hwnd), GWLP_USERDATA);
			
			char string[32];
			::GetWindowText(hwnd, string, 32);

			pw->_child_hwnds[(int)::GetMenu(hwnd)].property->SetByString(String(string));
		}
		else return ::CallWindowProc(_defaultEditProc, hwnd, msg, wparam, lparam);

		break;

	default:
		return ::CallWindowProc(_defaultEditProc, hwnd, msg, wparam, lparam);
	}

	return 0;
}

//static
void PropertyWindow::Initialise(HBRUSH brush)
{
	WNDCLASSEXA windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.lpfnWndProc = _WindowProc;
	windowClass.hInstance = ::GetModuleHandle(NULL);
	windowClass.hbrBackground = brush;
	windowClass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = _className;
	::RegisterClassEx(&windowClass);
}

PropertyWindow::PropertyWindow(Editor* owner) : _owner(owner)
{
}


PropertyWindow::~PropertyWindow()
{
}

void PropertyWindow::_CreateHWNDs(bool readOnly)
{
	auto properties = _objProperties.GetAll();

	RECT rect;
	::GetClientRect(_hwnd, &rect);

	const int w = rect.right / 2;
	int y = 0;

	for (uint32 i = 0; i < properties.GetSize(); ++i)
	{
		const char *name = properties[i].first.GetData();

		HINSTANCE instance = ::GetModuleHandle(NULL);

		HWND label = ::CreateWindow(WC_STATIC, name, WS_CHILD | WS_VISIBLE, 0, y, w, boxH, _hwnd, (HMENU)i, instance, NULL);

		HWND button = 0;
		if (properties[i].second->GetFlags() & (PropertyFlags::MODEL | PropertyFlags::MATERIAL))
			button = ::CreateWindow(WC_BUTTON, "...", WS_CHILD | WS_VISIBLE, w * 2 - buttonW, y, 20, boxH, _hwnd, (HMENU)i, instance, NULL);

		DWORD boxStyle = ES_LOWERCASE | WS_CHILD | WS_VISIBLE;
		if (readOnly) boxStyle |= ES_READONLY;

		HWND box = ::CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, properties[i].second->GetAsString().GetData(), boxStyle, w, y, w - (button ? buttonW : 0), boxH, _hwnd, (HMENU)i, instance, NULL);

		if (!_defaultEditProc)
			_defaultEditProc = (WNDPROC)::GetWindowLongPtr(box, GWLP_WNDPROC);

		::SetWindowLongPtr(box, GWLP_WNDPROC, (LONG_PTR)_EditProc);

		_child_hwnds.Add(PropertyHWND{ label, box, button, *_objProperties.FindRaw(name) });

		y += boxH;
	}
}

void PropertyWindow::SetProperties(const ObjectProperties &properties, bool readOnly)
{
	Clear();

	_objProperties = properties;
	_CreateHWNDs(readOnly);
}

void PropertyWindow::SetObject(GameObject *object, bool readOnly)
{
	Clear();

	_objProperties.SetBase(object);
	object->GetProperties(_objProperties);
	_CreateHWNDs(readOnly);
}

void PropertyWindow::ChangeBase(GameObject *object)
{
	_objProperties.SetBase(object);
	Refresh();
}

void PropertyWindow::Refresh()
{
	for (uint32 i = 0; i < _child_hwnds.GetSize(); ++i)
		::SetWindowTextA(_child_hwnds[i].box, _child_hwnds[i].property->GetAsString().GetData());}

void PropertyWindow::Clear()
{
	_objProperties.Clear(); 
	
	for (uint32 i = 0; i < _child_hwnds.GetSize(); ++i)
	{
		::DestroyWindow(_child_hwnds[i].box);
		::DestroyWindow(_child_hwnds[i].label);

		if (_child_hwnds[i].button)
			::DestroyWindow(_child_hwnds[i].button);
	}

	_child_hwnds.SetSize(0);
}
