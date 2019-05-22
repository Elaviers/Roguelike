#pragma once
#include <Engine/Window.hpp>
#include <Engine/ModelManager.hpp>
#include <Engine/ObjCamera.hpp>
#include <Engine/TextureManager.hpp>
#include <Windows.h>

class Editor;

class Viewport : public Window
{
private:
	static LPCTSTR _className;

	static LRESULT _WindowProc(HWND, UINT, WPARAM, LPARAM);

	Editor *_editor;
	int _index;

	ObjCamera _camera;

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

	inline void SetSizeAndPos(uint16 x, uint16 y, uint16 width, uint16 height) { WindowFunctions::SetHWNDSizeAndPos(_hwnd, x, y, width, height); _camera.SetViewport(width, height); }

	inline ObjCamera& CameraRef() { return _camera; }
};
