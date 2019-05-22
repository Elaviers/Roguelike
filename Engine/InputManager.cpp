#include "InputManager.hpp"
#include "String.hpp"
#include <Windows.h>

#define KEYNAME(KEY, NAME) {Keycode::KEY, #NAME}

const struct KeyName { Keycode key; String name; } keyNames[] =
{
	KEYNAME(A, a), KEYNAME(B, b), KEYNAME(C, c), KEYNAME(D, d), KEYNAME(E, e), KEYNAME(F, f), KEYNAME(G, g), KEYNAME(H, h), KEYNAME(I, i), KEYNAME(J, j), KEYNAME(K, k), KEYNAME(L, l), KEYNAME(M, m),
	KEYNAME(N, n), KEYNAME(O, o), KEYNAME(P, p), KEYNAME(Q, q), KEYNAME(R, r), KEYNAME(S, s), KEYNAME(T, t), KEYNAME(U, u), KEYNAME(V, v), KEYNAME(W, w), KEYNAME(X, x), KEYNAME(Y, y), KEYNAME(Z, z),
	KEYNAME(NUM0, 0), KEYNAME(NUM1, 1),KEYNAME(NUM2, 2),KEYNAME(NUM3, 3),KEYNAME(NUM4, 4),KEYNAME(NUM5, 5),KEYNAME(NUM6, 6),KEYNAME(NUM7, 7),KEYNAME(NUM8, 8), KEYNAME(NUM9, 9),

	KEYNAME(SPACE, space),
	KEYNAME(CTRL, ctrl),
	KEYNAME(SHIFT, shift),
	KEYNAME(ALT, alt),
	//KEYNAME(LCTRL, lctrl), KEYNAME(LSHIFT, lshift), KEYNAME(LALT, lalt),
	//KEYNAME(RCTRL, rctrl), KEYNAME(RSHIFT, rshift), KEYNAME(RALT, ralt),
	KEYNAME(ENTER, enter),
	KEYNAME(ESCAPE, escape),
	KEYNAME(CAPSLOCK, caps),
	KEYNAME(TAB, tab),
	KEYNAME(BACKSPACE, backspace),

	KEYNAME(SEMICOLON, semicolon),
	KEYNAME(SLASH, / ),
	KEYNAME(TILDE, ~),
	KEYNAME(SQBRACKETLEFT, [),
	KEYNAME(BACKSLASH, \\),
	KEYNAME(SQBRACKETRIGHT, ]),
	{ Keycode::APOSTROPHE, "\'" },
	{ Keycode::COMMA, ","},
	{ Keycode::PERIOD, '.'},

	KEYNAME(INSERT, insert), KEYNAME(HOME, home), KEYNAME(PAGEUP, pageup),
	KEYNAME(DEL, delete), KEYNAME(END, end), KEYNAME(PAGEDOWN, pagedown),

	KEYNAME(UP, up), KEYNAME(RIGHT, right), KEYNAME(LEFT, left), KEYNAME(DOWN, down)
};

const uint16 keyCount = sizeof(keyNames) / sizeof(KeyName);

String KeyToString(Keycode key)
{
	for (uint16 i = 0; i < keyCount; ++i)
		if (keyNames[i].key == key)
			return keyNames[i].name;

	return "";
}

Keycode StringToKey(const char *string)
{
	size_t length = StringLength(string);

	for (uint16 i = 0; i < keyCount; ++i)
		if (StringsEqual(string, keyNames[i].name.GetData()))
			return keyNames[i].key;

	return Keycode::NONE;
}

InputManager::~InputManager()
{
	_keyBinds.ForEach( [](const Keycode&, KeyBind* &keyBind)
	{
		delete keyBind;
	}
	);
}

void InputManager::KeyDown(Keycode key)
{
	_keyStates[(byte)key] = 1;

	KeyBind **keyBind = _keyBinds.Find(key);

	if (keyBind)
		(*keyBind)->KeyDown();
}

void InputManager::KeyUp(Keycode key)
{
	_keyStates[(byte)key] = 0;

	KeyBind **keyBind = _keyBinds.Find(key);

	if (keyBind)
		(*keyBind)->KeyUp();
}

void InputManager::MouseMove(short x, short y)
{
	auto mouseX = _axisBinds.Find(AxisType::MOUSE_X);
	auto mouseY = _axisBinds.Find(AxisType::MOUSE_Y);

	if (mouseX)
		**mouseX = (float)x;

	if (mouseY)
		**mouseY = (float)-y;
}
