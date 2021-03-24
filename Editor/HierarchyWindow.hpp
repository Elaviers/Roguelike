#pragma once
#include <Engine/WorldObject.hpp>
#include <ELCore/SharedPointer.hpp>
#include <ELSys/Window_Win32.hpp>
#include <CommCtrl.h>

class Editor;
class World;

/*
	HierarchyWindow

	This class was castrated when I rewrote the world
	TreeView is ugly/slow so I'm using my own UI / ImGui instead
	This is not currently being used anywhere!
*/

class HierarchyWindow : public Window_Win32
{
	static const TCHAR* _className;
	static LRESULT _WindowProc(HWND, UINT, WPARAM, LPARAM);

	SharedPointer<WorldObject> _dragObj;

	HWND _treeView;
	HIMAGELIST _imageList;

	Editor* _owner;

	HTREEITEM _AddObject(WorldObject&, HTREEITEM parent, HTREEITEM after);

public:
	HierarchyWindow(Editor *owner) : 
		_treeView(0), 
		_imageList(0),
		_owner(owner), 
		_dragObj()
	{}

	virtual ~HierarchyWindow() {}

	static void Initialise(HBRUSH);

	void Create(const Window_Win32* parent)
	{
		Window_Win32::Create(_className, TEXT("Hierarchy"), parent ? WS_CHILD : 0, parent ? parent->GetHWND() : NULL, this);
		Window_Win32::SetSize(256, 512);
	}

	void Refresh();

	void BeginDrag(LPNMTREEVIEW);
	void MouseMove(int x, int y);
	void MouseUp();
};

