#include "HierachyWindow.hpp"
#include "Editor.hpp"
#include "resource.h"
#include <Engine/Engine.hpp>
#include <Engine/Entity.hpp>
#include <CommCtrl.h>
#include <windowsx.h>

LPCTSTR HierachyWindow::_className = TEXT("HWCLASS");

//static
LRESULT HierachyWindow::_WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HierachyWindow* hw = (HierachyWindow*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT create = (LPCREATESTRUCT)lparam;
		hw = (HierachyWindow*)create->lpCreateParams;
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)hw);

		RECT rect;
		::GetClientRect(hwnd, &rect);

		HINSTANCE instance = ::GetModuleHandle(NULL);

		hw->_treeView = ::CreateWindowEx(0, WC_TREEVIEW, TEXT("Hierachy Tree"), WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASBUTTONS | TVS_HASLINES | TVS_SHOWSELALWAYS, 
			0, 0, rect.right, rect.bottom, hwnd, (HMENU)1, instance, NULL);

		const int imageCount = 2;

		hw->_imageList = ::ImageList_Create(16, 16, FALSE, imageCount, 0);

		HBITMAP bmp = ::LoadBitmap(instance, MAKEINTRESOURCE(IDB_TREENODE));
		::ImageList_Add(hw->_imageList, bmp, NULL);
		::DeleteObject(bmp);

		bmp = ::LoadBitmap(instance, MAKEINTRESOURCE(IDB_TREENODE2));
		::ImageList_Add(hw->_imageList, bmp, NULL);
		::DeleteObject(bmp);

		if (::ImageList_GetImageCount(hw->_imageList) < imageCount)
			Debug::Error("Oh dear. Somehow the tree view's imagelist could not be created properly.");

		::SendMessage(hw->_treeView, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, (LPARAM)hw->_imageList);
	}
	break;

	case WM_CLOSE:
		::ShowWindow(hwnd, SW_HIDE);
		break;

	case WM_SIZE:
	{
		SHORT w = LOWORD(lparam);
		SHORT h = HIWORD(lparam);

		WindowFunctions::ResizeHWND(hw->_treeView, w, h);
	}
	break;

	case WM_MOUSEMOVE:
		hw->MouseMove(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		break;

	case WM_LBUTTONUP:
		hw->MouseUp();
		break;

	case WM_NOTIFY:
	{
		LPNMHDR notify = (LPNMHDR)lparam;

		switch (notify->code)
		{
		case TVN_SELCHANGED:
		{
			hw->_owner->SetTool(TOOL_SELECT);

			LPNMTREEVIEW nmtv = (LPNMTREEVIEW)lparam;

			hw->_owner->tools.select.Select(hw->_currentRoot->FindByUID((uint32)nmtv->itemNew.lParam));
		}
			break;

		case TVN_BEGINDRAG:
			hw->BeginDrag((LPNMTREEVIEW)lparam);

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
void HierachyWindow::Initialise(HBRUSH brush)
{
	WNDCLASSEXA windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.lpfnWndProc = (WNDPROC)_WindowProc;
	windowClass.hInstance = ::GetModuleHandle(NULL);
	windowClass.hbrBackground = brush;
	windowClass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = _className;
	::RegisterClassEx(&windowClass);
}

HTREEITEM HierachyWindow::_AddObject(Entity& obj, HTREEITEM parent, HTREEITEM after)
{
	auto regNode = Engine::Instance().registry.GetNode(obj.GetTypeID());
	String name = regNode ? regNode->name : "???";

	if (obj.GetName().GetLength())
		name += String(" - \"") + obj.GetName() + '\"';

	TVINSERTSTRUCT insertStruct = {};
	insertStruct.hParent = parent;
	insertStruct.hInsertAfter = after;
	insertStruct.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	insertStruct.item.iImage = 0;
	insertStruct.item.iSelectedImage = 1;
	insertStruct.item.lParam = (LPARAM)obj.GetUID();
	insertStruct.item.pszText = &name[0];
	insertStruct.item.cchTextMax = (int)name.GetLength();
	insertStruct.item.state = 0xFF;
	insertStruct.item.stateMask = TVIS_EXPANDED;

	HTREEITEM item = (HTREEITEM)::SendMessage(_treeView, TVM_INSERTITEM, 0, (LPARAM)&insertStruct);
	HTREEITEM prev = TVI_FIRST;

	auto children = obj.Children();
	for (size_t i = 0; i < children.GetSize(); ++i)
		prev = _AddObject(*children[i], item, prev);

	return item;
}

void HierachyWindow::Refresh(Entity& root)
{
	_currentRoot = Engine::Instance().pObjectTracker->Track(&root);

	TreeView_DeleteAllItems(_treeView);

	_AddObject(root, TVI_ROOT, TVI_ROOT);
}

void HierachyWindow::BeginDrag(LPNMTREEVIEW nmtv)
{
	HIMAGELIST himl;    // handle to image list 
	RECT rcItem;        // bounding rectangle of item 

	// Tell the tree-view control to create an image to use 
	// for dragging. 
	himl = TreeView_CreateDragImage(_treeView, nmtv->itemNew.hItem);

	// Get the bounding rectangle of the item being dragged. 
	TreeView_GetItemRect(_treeView, nmtv->itemNew.hItem, &rcItem, TRUE);

	// Start the drag operation. 
	ImageList_BeginDrag(himl, 0, 0, 0);
	ImageList_DragEnter(_treeView, nmtv->ptDrag.x, nmtv->ptDrag.x);

	// Hide the mouse pointer, and direct mouse input to the 
	// parent window. 
	ShowCursor(FALSE);
	SetCapture(GetParent(_treeView));
	_dragObj = Engine::Instance().pObjectTracker->Track(_currentRoot->FindByUID((uint32)nmtv->itemNew.lParam));
}

void HierachyWindow::MouseMove(int x, int y)
{
	if (_dragObj)
	{
		HTREEITEM htiTarget;  // Handle to target item. 
		TVHITTESTINFO tvht;   // Hit test information. 

		// Drag the item to the current position of the mouse pointer. 
		// First convert the dialog coordinates to control coordinates. 
		POINT point;
		point.x = x;
		point.y = y;
		ClientToScreen(_hwnd, &point);
		ScreenToClient(_treeView, &point);
		ImageList_DragMove(point.x, point.y);
		// Turn off the dragged image so the background can be refreshed.
		ImageList_DragShowNolock(FALSE);

		// Find out if the pointer is on the item. If it is, 
		// highlight the item as a drop target. 
		tvht.pt.x = point.x;
		tvht.pt.y = point.y;
		if ((htiTarget = TreeView_HitTest(_treeView, &tvht)) != NULL)
		{
			TreeView_SelectDropTarget(_treeView, htiTarget);
		}
		ImageList_DragShowNolock(TRUE);
	}
}

void HierachyWindow::MouseUp()
{
	if (_dragObj)
	{
		// Get destination item.
		HTREEITEM htiDest = TreeView_GetDropHilight(_treeView);
		if (htiDest != NULL)
		{
			TVITEM item = {};
			item.hItem = htiDest;
			item.mask = TVIF_PARAM;
			
			TreeView_GetItem(_treeView, &item);

			Entity* go = _currentRoot->FindByUID((uint32)item.lParam);

			if (go != _dragObj.Ptr() && !go->IsChildOf(_dragObj.Ptr()))
			{
				_dragObj->SetParent(go);
				Refresh(*_currentRoot);
			}
		}

		ImageList_EndDrag();
		TreeView_SelectDropTarget(_treeView, NULL);
		ReleaseCapture();
		ShowCursor(TRUE);
		_dragObj.Clear();
	}

}
