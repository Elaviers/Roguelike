#pragma once
#include "Map.h"
#include <Windows.h>

enum class Keycode
{
	NONE = 0,

	A = 'A', B = 'B', C = 'C', D = 'D', E = 'E', F = 'F', G = 'G', H = 'H', I = 'I', J = 'J', K = 'K', L = 'L', M = 'M', 
	N = 'N', O = 'O', P = 'P', Q = 'Q', R = 'R', S = 'S', T = 'T', U = 'U', V = 'V', W = 'W', X = 'X', Y = 'Y', Z = 'Z',

	NUM0 = '0', NUM1 = '1', NUM2 = '2', NUM3 = '3', NUM4 = '4', NUM5 = '5', NUM6 = '6', NUM7 = '7', NUM8 = '8', NUM9 = '9',
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

	SPACE = VK_SPACE,
	LCTRL = VK_LCONTROL, LSHIFT = VK_LSHIFT, LALT = VK_LMENU,
	RCTRL = VK_RCONTROL, RSHIFT = VK_RSHIFT, RALT = VK_RMENU,
	ENTER = VK_RETURN,
	CAPSLOCK = VK_CAPITAL,
	TAB = VK_TAB,
	BACKSPACE = VK_BACK,

	SEMICOLON = VK_OEM_1,
	SLASH = VK_OEM_2,
	TILDE = VK_OEM_3,
	SQBRACKETLEFT = VK_OEM_4,
	BACKSLASH = VK_OEM_5,
	SQBRACKETRIGHT = VK_OEM_6,
	APOSTROPHE = VK_OEM_7,
	COMMA = VK_OEM_COMMA,
	PERIOD = VK_OEM_PERIOD,

	INSERT = VK_INSERT, HOME = VK_HOME, PAGEUP = VK_PRIOR,
	DEL = VK_DELETE, END = VK_END, PAGEDOWN = VK_NEXT,
	UP = VK_UP, RIGHT = VK_RIGHT, DOWN = VK_DOWN, LEFT = VK_LEFT,
};

enum class AxisType
{
	MOUSE_X, MOUSE_Y,
	LJOY_X, LJOY_Y,
	RJOY_X, RJOY_Y,
	LTRIGGER, RTRIGGER
};

struct KeyBind
{
	float *axis;	//If null, callback must be set

	union
	{
		float displacement;
		void (*callback)();
	};

	KeyBind() {};
	KeyBind(void(*callback)()) : axis(nullptr), callback(callback) {}
	KeyBind(float *axis, float displacement) : axis(axis), displacement(displacement) {}
};

class InputManager
{
private:
	Map<Keycode, KeyBind> _keyBinds;
	Map<AxisType, float*> _axisBinds;

public:
	InputManager() {}
	~InputManager() {}

	inline void BindAxis(AxisType axis, float *axisPtr) { _axisBinds.Set(axis, axisPtr); }
	inline void BindKey(Keycode key, void(*callback)()) { _keyBinds.Set(key, KeyBind(callback)); }
	inline void BindKeyAxis(Keycode key, float *axisPtr, float axisDisplacement) { _keyBinds.Set(key, KeyBind(axisPtr, axisDisplacement)); };

	void KeyDown(Keycode);
	void KeyUp(Keycode);

	//xDelta and yDelta are in pixels
	void MouseMove(short xDelta, short yDelta);
};
