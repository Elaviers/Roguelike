#pragma once
#include <ELCore/SharedPointer.hpp>
#include <ELSys/Window_Win32.hpp>
#include <CommCtrl.h>

class Entity;
class Editor;

class HierachyWindow : public Window_Win32
{
	static const TCHAR* _className;
	static LRESULT _WindowProc(HWND, UINT, WPARAM, LPARAM);

	EntityPointer _currentRoot;
	EntityPointer _dragObj;

	HWND _treeView;
	HIMAGELIST _imageList;

	Editor* _owner;

	HTREEITEM _AddObject(Entity&, HTREEITEM parent, HTREEITEM after);

public:
	HierachyWindow(Editor *owner) : 
		_treeView(0), 
		_imageList(0),
		_owner(owner), 
		_dragObj(), 
		_currentRoot() 
	{}

	virtual ~HierachyWindow() {}

	static void Initialise(HBRUSH);

	void Create(const Window_Win32* parent)
	{
		Window_Win32::Create(_className, TEXT("Hierachy"), this, parent ? WS_CHILD : 0, parent ? parent->GetHWND() : NULL);
		Window_Win32::SetSize(256, 512);
	}

	void Refresh(Entity& root);

	void BeginDrag(LPNMTREEVIEW);
	void MouseMove(int x, int y);
	void MouseUp();
};

