#pragma once
#include <Engine/Window.hpp>
#include <Engine/Buffer.hpp>
#include <Engine/CvarMap.hpp>

class Editor;
class GameObject;

struct PropertyHWND
{
	HWND label;
	HWND box;
	HWND button;
	
	Cvar *cvar;
};

class PropertyWindow : public Window
{
	static LPCTSTR _className;
	static LRESULT _WindowProc(HWND, UINT, WPARAM, LPARAM);

	static LRESULT _EditProc(HWND, UINT, WPARAM, LPARAM);
	static WNDPROC _defaultEditProc;

	CvarMap _cvars;

	Buffer<PropertyHWND> _child_hwnds;

	Editor* const _owner;

	void _CreateHWNDs(bool readOnly);

public:
	PropertyWindow(Editor* owner) : _owner(owner) {}
	virtual ~PropertyWindow() {}

	static void Initialise(HBRUSH);

	inline void Create(const Window &parent)
	{
		Window::Create(_className, TEXT("Properties"), this, WS_CHILD, parent.GetHwnd());
		Window::SetSize(256, 512);
	}

	inline CvarMap& GetCvars() { return _cvars; }
	void SetCvars(const CvarMap&, bool readOnly = false);

	void SetObject(GameObject*, bool readOnly = false);

	void Refresh();

	void Clear();
};
