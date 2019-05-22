#include "PropertyWindow.hpp"
#include <CommCtrl.h>
#include <Engine/GameObject.hpp>
#include <Engine/Registry.hpp>
#include "Editor.hpp"

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
		switch (HIWORD(wparam))
		{
		case EN_KILLFOCUS:
		{
			char string[32];
			::GetWindowText((HWND)lparam, string, 32);

			pw->_child_hwnds[LOWORD(wparam)].cvar->SetByString(String(string));
		}
			break;

		case BN_CLICKED:
		{
			uint32 id = LOWORD(wparam);

			String string;

			if (pw->_child_hwnds[id].cvar->GetFlags() & PropertyFlags::MATERIAL)
				string = pw->_owner->SelectMaterialDialog();
			else if (pw->_child_hwnds[id].cvar->GetFlags() & PropertyFlags::MODEL)
				string = pw->_owner->SelectModelDialog();
			else break;

			if (string.GetLength() > 0)
			{
				::SetWindowTextA(pw->_child_hwnds[id].box, string.GetData());
				pw->_child_hwnds[id].cvar->SetByString(string);
			}
		}
			break;

		case CBN_SELCHANGE:
		{
			LRESULT cbIndex = ::SendMessage((HWND)lparam, CB_GETCURSEL, 0, 0);

			if (pw->_child_hwnds[LOWORD(wparam)].cvar->GetFlags() & PropertyFlags::CLASSID)
			{
				auto regTypes = Engine::Instance().registry.GetRegisteredTypes();
				reinterpret_cast<TypedCvar<byte>*>(pw->_child_hwnds[LOWORD(wparam)].cvar)->Set(*regTypes[cbIndex].first);
			}
			else
			{
				char selectedString[256];
				::SendMessage((HWND)lparam, CB_GETLBTEXT, (WPARAM)cbIndex, (LPARAM)selectedString);

				pw->_child_hwnds[LOWORD(wparam)].cvar->SetByString(selectedString);
			}
		}
			break;
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

			pw->_child_hwnds[(size_t)::GetMenu(hwnd)].cvar->SetByString(String(string));
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

void PropertyWindow::_CreateHWNDs(bool readOnly)
{
	auto properties = _cvars.GetAll();

	RECT rect;
	::GetClientRect(_hwnd, &rect);

	const int w = rect.right / 2;
	int y = 0;

	for (uint32 i = 0; i < properties.GetSize(); ++i)
	{
		const char *name = properties[i].first->GetData();

		HINSTANCE instance = ::GetModuleHandle(NULL);

		HWND label = ::CreateWindow(WC_STATIC, name, WS_CHILD | WS_VISIBLE, 0, y, w, boxH, _hwnd, (HMENU)i, instance, NULL);

		HWND box, button = 0;

		if ((*properties[i].second)->GetFlags() & PropertyFlags::CLASSID && (*properties[i].second)->GetType() == CvarType::BYTE)
		{
			box = ::CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOXA, (*properties[i].second)->GetAsString().GetData(), WS_CHILD | WS_VISIBLE | CBS_HASSTRINGS | CBS_DROPDOWNLIST, w, y, w, boxH, _hwnd, (HMENU)i, instance, NULL);

			byte current = reinterpret_cast<TypedCvar<byte>*>((*properties[i].second))->Get();

			auto listItems = Engine::Instance().registry.GetRegisteredTypes();
			for (uint32 i = 0; i < listItems.GetSize(); ++i)
			{
				::SendMessage(box, CB_ADDSTRING, 0, (LPARAM)Engine::Instance().registry.GetNode(*listItems[i].first)->name.GetData());

				if (*listItems[i].first == current)
					::SendMessage(box, CB_SETCURSEL, i, 0);
			}
		}
		else if ((*properties[i].second)->GetFlags() & PropertyFlags::DIRECTION)
		{
			box = ::CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOXA, (*properties[i].second)->GetAsString().GetData(), WS_CHILD | WS_VISIBLE | CBS_HASSTRINGS | CBS_DROPDOWNLIST, w, y, w, boxH, _hwnd, (HMENU)i, instance, NULL);

			::SendMessage(box, CB_ADDSTRING, 0, (LPARAM)"north");
			::SendMessage(box, CB_ADDSTRING, 0, (LPARAM)"east");
			::SendMessage(box, CB_ADDSTRING, 0, (LPARAM)"south");
			::SendMessage(box, CB_ADDSTRING, 0, (LPARAM)"west");

			String value = (*properties[i].second)->GetAsString();

			if (value == "north")
				::SendMessage(box, CB_SETCURSEL, 0, 0);
			else if (value == "east")
				::SendMessage(box, CB_SETCURSEL, 1, 0);
			else if (value == "south")
				::SendMessage(box, CB_SETCURSEL, 2, 0);
			else if (value == "west")
				::SendMessage(box, CB_SETCURSEL, 3, 0);
		}
		else
		{
			if (!readOnly && (*properties[i].second)->GetFlags() & (PropertyFlags::MODEL | PropertyFlags::MATERIAL))
				button = ::CreateWindow(WC_BUTTON, "...", WS_CHILD | WS_VISIBLE, w * 2 - buttonW, y, 20, boxH, _hwnd, (HMENU)i, instance, NULL);

			DWORD boxStyle = ES_LOWERCASE | WS_CHILD | WS_VISIBLE;
			if (readOnly) boxStyle |= ES_READONLY;

			box = ::CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, (*properties[i].second)->GetAsString().GetData(), boxStyle, w, y, w - (button ? buttonW : 0), boxH, _hwnd, (HMENU)i, instance, NULL);

			if (!_defaultEditProc)
				_defaultEditProc = (WNDPROC)::GetWindowLongPtr(box, GWLP_WNDPROC);

			::SetWindowLongPtr(box, GWLP_WNDPROC, (LONG_PTR)_EditProc);
		}

		_child_hwnds.Add(PropertyHWND{ label, box, button, _cvars.FindRaw(name) });

		y += boxH;
	}
}

void PropertyWindow::SetCvars(const CvarMap &cvars, bool readOnly)
{
	Clear();

	_cvars = cvars;
	_CreateHWNDs(readOnly);
}

void PropertyWindow::SetObject(GameObject *object, bool readOnly)
{
	Clear();

	object->GetCvars(_cvars);
	_CreateHWNDs(readOnly);
}

void PropertyWindow::Refresh()
{
	for (uint32 i = 0; i < _child_hwnds.GetSize(); ++i)
		::SetWindowTextA(_child_hwnds[i].box, _child_hwnds[i].cvar->GetAsString().GetData());}

void PropertyWindow::Clear()
{
	_cvars.Clear(); 
	
	for (uint32 i = 0; i < _child_hwnds.GetSize(); ++i)
	{
		::DestroyWindow(_child_hwnds[i].box);
		::DestroyWindow(_child_hwnds[i].label);

		if (_child_hwnds[i].button)
			::DestroyWindow(_child_hwnds[i].button);
	}

	_child_hwnds.SetSize(0);
}
