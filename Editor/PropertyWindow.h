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
	
	PropertyPointer property;
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

public:
	PropertyWindow(Editor *owner);
	virtual ~PropertyWindow();

	static void Initialise();

	inline void Create()
	{
		Window::Create(_className, TEXT("Properties"), this);
		Window::SetSize(256, 512);

		//_font = ::CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Arial"));
	}

	void SetObject(GameObject*);
};

