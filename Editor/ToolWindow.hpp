#pragma once
#include <Engine/Window.hpp>
#include "PropertyWindow.hpp"

class Editor;

class ToolWindow : public Window
{
	static const TCHAR* _className;
	static LRESULT _WindowProc(HWND, UINT, WPARAM, LPARAM);

	PropertyWindow _propertyWindow;

public:
	ToolWindow(Editor *owner) : _propertyWindow(owner) {}
	virtual ~ToolWindow() {}

	static void Initialise(HBRUSH);

	void Create(const Window &parent)
	{
		Window::Create(_className, TEXT("Tool"), this, WS_CHILD, parent.GetHwnd());
		_propertyWindow.Create(*this);
		_propertyWindow.SetSizeAndPos(12,24,224,248);
	}

	PropertyWindow& PropertyWindow() { return _propertyWindow; }
};

