#pragma once
#include <Engine/Window.hpp>
#include <Engine/SharedPointer.hpp>
#include <CommCtrl.h>

class GameObject;
class Editor;

class HierachyWindow : public Window
{
	static const TCHAR* _className;
	static LRESULT _WindowProc(HWND, UINT, WPARAM, LPARAM);

	GameObjectPointer _currentRoot;
	GameObjectPointer _dragObj;

	HWND _treeView;
	HIMAGELIST _imageList;

	Editor* _owner;

	HTREEITEM _AddObject(GameObject&, HTREEITEM parent, HTREEITEM after);

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

	inline void Create(const Window* parent)
	{
		Window::Create(_className, TEXT("Hierachy"), this, parent ? WS_CHILD : 0, parent ? parent->GetHwnd() : NULL);
		Window::SetSize(256, 512);
	}

	void Refresh(GameObject& root);

	void BeginDrag(LPNMTREEVIEW);
	void MouseMove(int x, int y);
	void MouseUp();
};

