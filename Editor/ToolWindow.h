#pragma once
#include <Engine/Window.h>
#include "PropertyWindow.h"

class Editor;

class ToolWindow : public Window
{
	static LPCTSTR _className;
	static LRESULT _WindowProc(HWND, UINT, WPARAM, LPARAM);

	PropertyWindow _propertyWindow;

public:
	ToolWindow(Editor *owner) : _propertyWindow(owner) {}
	virtual ~ToolWindow() {}

	static void Initialise(HBRUSH);

	inline void Create(const Window &parent)
	{
		Window::Create(_className, TEXT("Tool"), this, WS_CHILD, parent.GetHwnd());
		_propertyWindow.Create(*this);
		_propertyWindow.SetSizeAndPos(12,24,224,248);
	}

	inline PropertyWindow& PropertyWindow() { return _propertyWindow; }
};

