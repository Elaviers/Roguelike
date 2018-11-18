#pragma once
#include <Engine/Window.h>
#include <Engine/Buffer.h>
#include <Engine/ObjectProperties.h>

class Editor;
class GameObject;

struct PropertyHWND
{
	HWND label;
	HWND box;
	HWND button;
	
	Property *property;
};

class PropertyWindow : public Window
{
	static LPCTSTR _className;
	static LRESULT _WindowProc(HWND, UINT, WPARAM, LPARAM);

	static LRESULT _EditProc(HWND, UINT, WPARAM, LPARAM);
	static WNDPROC _defaultEditProc;

	ObjectProperties _objProperties;

	Buffer<PropertyHWND> _child_hwnds;

	Editor* const _owner;

	void _CreateHWNDs(bool readOnly);

public:
	PropertyWindow(Editor *owner);
	virtual ~PropertyWindow();

	static void Initialise(HBRUSH);

	inline void Create(const Window &parent)
	{
		Window::Create(_className, TEXT("Properties"), this, WS_CHILD, parent.GetHwnd());
		Window::SetSize(256, 512);
	}

	inline ObjectProperties& GetProperties() { return _objProperties; }
	void SetProperties(const ObjectProperties&, bool readOnly = false);

	void SetObject(GameObject*, bool readOnly = false);
	void ChangeBase(GameObject*);

	void Refresh();

	void Clear();
};
