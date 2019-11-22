#pragma once
#include <Engine/Window.hpp>
#include <Engine/Buffer.hpp>
#include <Engine/Entity.hpp>

class Editor;

struct PropertyHWND
{
	HWND label;
	HWND box;
	HWND button;
	
	Property *property;
};

class PropertyWindow : public Window
{
	static const TCHAR* _className;
	static LRESULT _WindowProc(HWND, UINT, WPARAM, LPARAM);

	static LRESULT _EditProc(HWND, UINT, WPARAM, LPARAM);
	static WNDPROC _defaultEditProc;

	const PropertyCollection* _cvars;
	void* _currentObject;

	Buffer<PropertyHWND> _child_hwnds;

	Editor* const _owner;

	void _CreateHWNDs(bool readOnly);

public:
	PropertyWindow(Editor* owner) : _owner(owner), _cvars(nullptr), _currentObject(nullptr) {}
	virtual ~PropertyWindow() {}

	static void Initialise(HBRUSH);

	void Create(const Window &parent)
	{
		Window::Create(_className, TEXT("Properties"), this, WS_CHILD, parent.GetHwnd());
		Window::SetSize(256, 512);
	}

	const PropertyCollection* GetProperties() { return _cvars; }
	void SetCvars(const PropertyCollection&, void* object, bool readOnly = false);

	void SetObject(Entity* object, bool readOnly = false) { SetCvars(object->GetProperties(), object, readOnly); }

	void Refresh();

	void Clear();
};
