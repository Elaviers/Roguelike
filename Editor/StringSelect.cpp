#include "StringSelect.hpp"
#include "resource.h"

struct StringSelectDialog
{
	const char* title;

	const Buffer<String>& strings;

	String result;
};

INT_PTR DialogProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	StringSelectDialog* ss = (StringSelectDialog*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_INITDIALOG:
		ss = (StringSelectDialog*)lparam;
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)ss);
		::SetWindowTextA(hwnd, ss->title);

		for (size_t i = 0; i < ss->strings.GetSize(); ++i)
			::SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM)ss->strings[i].begin());

		break;

	case WM_CLOSE:
		::EndDialog(hwnd, 0);
		break;
		
	case WM_COMMAND:
		switch (HIWORD(wparam))
		{
		case LBN_SELCHANGE:

			int selIndex = (int)::SendDlgItemMessage(hwnd, IDC_LIST, LB_GETCURSEL, 0, 0);

			if (selIndex == LB_ERR)
				ss->result = "";
			else
			{
				int length = (int)::SendDlgItemMessage(hwnd, IDC_LIST, LB_GETTEXTLEN, (WPARAM)selIndex, 0);
				
				char* string = new char[(size_t)length + 1];
				::SendDlgItemMessage(hwnd, IDC_LIST, LB_GETTEXT, (WPARAM)selIndex, (LPARAM)string);
				ss->result = string;
				delete[] string;
			}

			break;
		}

		switch (LOWORD(wparam))
		{
		case IDOK:
			::EndDialog(hwnd, 1);
			break;

		case IDCANCEL:
			::EndDialog(hwnd, 0);
			break;
		}

		break;

	default:
		return FALSE;
	}

	return TRUE;
}

String StringSelect::Dialog(HWND parent, const Buffer<String>& strings, const char* title)
{
	StringSelectDialog dlg = { title, strings };

	if (::DialogBoxParam(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LIST_SELECT), parent, DialogProc, (LPARAM)& dlg) == 1)
		return dlg.result;

	return "";
}
