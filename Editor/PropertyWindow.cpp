#include "PropertyWindow.h"
#include <CommCtrl.h>
#include <Engine/GameObject.h>

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
		::DestroyWindow(hwnd);
		break;

	case WM_SIZE:
	{
		SHORT w = LOWORD(lparam);

		uint16 label_w = w / 2;
		uint16 box_w = w / 2;
		const uint16 h = 18;
		uint16 y = h;

		for (int i = 0; i < pw->_child_hwnds.GetSize(); ++i)
		{
			WindowFunctions::SetHWNDSizeAndPos(pw->_child_hwnds[i].label, 0, y, label_w, h);
			WindowFunctions::SetHWNDSizeAndPos(pw->_child_hwnds[i].box, label_w, y, box_w, h);

			y += h;
		}
	}
		break;

	case WM_COMMAND:
	{
		if (HIWORD(wparam) == EN_KILLFOCUS)
		{
			char string[32];
			::GetWindowText((HWND)lparam, string, 32);

			pw->_child_hwnds[LOWORD(wparam)].property.SetByString(String(string));
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

			pw->_child_hwnds[(int)::GetMenu(hwnd)].property.SetByString(String(string));
		}
		else return ::CallWindowProc(_defaultEditProc, hwnd, msg, wparam, lparam);

		break;

	default:
		return ::CallWindowProc(_defaultEditProc, hwnd, msg, wparam, lparam);
	}

	return 0;
}

//static
void PropertyWindow::Initialise()
{
	WNDCLASSEXA windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.lpfnWndProc = _WindowProc;
	windowClass.hInstance = ::GetModuleHandle(NULL);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = _className;
	::RegisterClassEx(&windowClass);
}

PropertyWindow::PropertyWindow()
{
}


PropertyWindow::~PropertyWindow()
{
}

void PropertyWindow::SetObject(GameObject *object)
{
	_child_hwnds.SetSize(0);

	_objProperties.Clear();
	_objProperties.SetObject(*object);
	object->GetProperties(_objProperties);

	auto properties = _objProperties.GetAll();

	const int w = 150;
	const int h = 18;
	int y = h;

	for (int i = 0; i < properties.GetSize(); ++i)
	{
		const char *name = properties[i].name.GetData();
		
		HWND label = ::CreateWindow(WC_STATIC, name, WS_CHILD | WS_VISIBLE, 0, y, w, h, this->GetHwnd(), (HMENU)i, ::GetModuleHandle(NULL), NULL);
		HWND box = ::CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, properties[i].propertyPointer.GetAsString().GetData(), WS_CHILD | WS_VISIBLE, w, y, w, h, this->GetHwnd(), (HMENU)i, ::GetModuleHandle(NULL), NULL);

		if (!_defaultEditProc)
			_defaultEditProc = (WNDPROC)::GetWindowLongPtr(box, GWLP_WNDPROC);

		::SetWindowLongPtr(box, GWLP_WNDPROC, (LONG_PTR)_EditProc);

		_child_hwnds.Add(PropertyHWND{label, box, *_objProperties.FindRaw(name)});

		y += h;
	}
}
