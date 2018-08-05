#pragma once
#include <Engine/Window.h>
#include <Engine/Camera.h>
#include <Engine/ModelManager.h>
#include <Engine/TextureManager.h>
#include <Windows.h>

class Editor;

class Viewport : public Window
{
private:
	static LPCTSTR _className;

	static LRESULT _WindowProc(HWND, UINT, WPARAM, LPARAM);

	Editor *_editor;
	int _index;

public:
	Viewport();
	~Viewport();

	inline void Create(HWND parent, Editor &editor, int index) 
	{ 
		Window::Create(_className, nullptr, this, WS_CHILD, parent); 
		_editor = &editor; 
		_index = index;
	}

	static void Initialise();
};
