#pragma once
#include "EKeycode.hpp"
#include "GL.hpp"
#include <ELCore/MacroUtilities.hpp>
#include <ELCore/Pool.hpp>
#include <ELCore/Types.hpp>
#include <Windows.h>

namespace WindowFunctions
{
	inline void SetHWNDSizeAndPos(HWND hwnd, uint16 x, uint16 y, uint16 w, uint16 h) { ::SetWindowPos(hwnd, NULL, x, y, w, h, 0); }
	void ResizeHWND(HWND, uint16 w, uint16 h);
	void RepositionHWND(HWND, uint16 x, uint16 y);

	void SetDefaultPixelFormat(HDC hdc);
}

struct WindowEvent
{
	enum Type
	{
		NONE = 0,
		CLOSED,
		RESIZE,
		FOCUS_GAINED,
		FOCUS_LOST,
		MOUSEMOVE,
		LEFTMOUSEDOWN,
		LEFTMOUSEUP,
		RIGHTMOUSEDOWN,
		RIGHTMOUSEUP,
		KEYDOWN,
		KEYUP,
		CHAR,
		RAWINPUT
	} type;

	enum class RawInputType
	{
		MOUSE
	};

	union _extra
	{
		struct _resize
		{
			uint16 w;
			uint16 h;
		} resize;

		struct _mouseMove
		{
			uint16 x;
			uint16 y;
		} mouseMove;

		struct _keyDown
		{
			EKeycode key;
			bool isRepeat;
		} keyDown;

		struct _keyUp
		{
			EKeycode key;
		} keyUp;

		char character;

		struct _rawInput
		{
			RawInputType type;

			struct _mouse
			{
				int16 lastX;
				int16 lastY;
			} mouse;
		} rawInput;
	} data;

	WindowEvent() {}
	WindowEvent(Type type) : type(type) {}
};

enum class WindowFlags : uint16
{
	NONE = 0,
	OPENGL = 0x0001,
	BORDERLESS = 0x0002,
	FULLSCREEN = 0x0004
};

DEFINE_BITMASK_FUNCS(WindowFlags, uint16)

class Window
{
protected:
	HWND	_hwnd;		//Window handle
	HDC		_hdc;		//Device context handle

	typedef MultiPool<byte, sizeof(WindowEvent) * 8> _EventPoolType;
	_EventPoolType _eventPool;
	List<WindowEvent> _eventList;

	static HINSTANCE _programInstance;
	static LPCTSTR _wclassDefault;
	static LPCTSTR _wclassDummy;

	static void _EnsureWindowClasses();

	friend class GLContext;
	friend void GL::LoadExtensions(const Window&);
	friend int WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

	//Yeah, that's right. This class is buddies with WinMain. :/

public:
	Window();

	virtual ~Window()
	{
		if (_hwnd) ::DestroyWindow(_hwnd);
	}

	void Create(const char* title, const Window* parent = nullptr, WindowFlags flags = WindowFlags::NONE);
	void Show();
	void Hide();
	void SwapBuffers();

	void Focus();

	void SetTitle(const char *title)									{ ::SetWindowTextA(_hwnd, title); }
	void SetSizeAndPos(uint16 x, uint16 y, uint16 width, uint16 height)	{ WindowFunctions::SetHWNDSizeAndPos(_hwnd, x, y, width, height); }
	void SetSize(uint16 width, uint16 height)							{ WindowFunctions::ResizeHWND(_hwnd, width, height); }
	void SetPos(uint16 x, uint16 y)										{ WindowFunctions::RepositionHWND(_hwnd, x, y); }

	bool PollEvent(WindowEvent& out);

	static LRESULT CALLBACK _WindowsProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};
