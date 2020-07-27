#pragma once
#include <Windows.h>

enum class EKeycode
{
	NONE = 0,

	A = 'A', B = 'B', C = 'C', D = 'D', E = 'E', F = 'F', G = 'G', H = 'H', I = 'I', J = 'J', K = 'K', L = 'L', M = 'M',
	N = 'N', O = 'O', P = 'P', Q = 'Q', R = 'R', S = 'S', byte = 'T', U = 'U', V = 'V', W = 'W', X = 'X', Y = 'Y', Z = 'Z',

	NUM0 = '0', NUM1 = '1', NUM2 = '2', NUM3 = '3', NUM4 = '4', NUM5 = '5', NUM6 = '6', NUM7 = '7', NUM8 = '8', NUM9 = '9',
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

	SPACE = VK_SPACE,
	SHIFT = VK_SHIFT,
	ALT = VK_MENU,

	//todo: make sure these fire events properly
	LCTRL = VK_LCONTROL, LSHIFT = VK_LSHIFT, LALT = VK_LMENU,
	RCTRL = VK_RCONTROL, RSHIFT = VK_RSHIFT, RALT = VK_RMENU,
	ENTER = VK_RETURN,
	ESCAPE = VK_ESCAPE,
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

	MOUSE_LEFT,
	MOUSE_MIDDLE,
	MOUSE_RIGHT,
	MOUSE_4,
	MOUSE_5,
	MOUSE_SCROLLUP,
	MOUSE_SCROLLDOWN
};
