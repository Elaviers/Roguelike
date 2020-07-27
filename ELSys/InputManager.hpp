#pragma once
#include "EKeycode.hpp"
#include <ELCore/FunctionPointer.hpp>
#include <ELCore/Hashmap.hpp>

enum class EAxis
{
	MOUSE_X, MOUSE_Y,
	LJOY_X, LJOY_Y,
	RJOY_X, RJOY_Y,
	LTRIGGER, RTRIGGER
};

class KeyBind
{
protected:
	KeyBind() {}

public:
	virtual ~KeyBind() {}

	virtual void KeyDown() {}
	virtual void KeyUp() {}
};

class KeyBind_Axis : public KeyBind
{
	float *_axis;
	float _displacement;

public:
	KeyBind_Axis(float *axis, float displacement) : _axis(axis), _displacement(displacement) {}
	virtual ~KeyBind_Axis() {}

	virtual void KeyDown()
	{
		*_axis += _displacement;
	}

	virtual void KeyUp()
	{
		*_axis -= _displacement;
	}
};

class KeyBind_Callback : public KeyBind
{
	Callback _callback;

public:
	KeyBind_Callback(const Callback &callback) : _callback(callback) {}
	virtual ~KeyBind_Callback() {}

	virtual void KeyDown()
	{
		_callback.TryCall();
	}
};

class InputManager
{
private:
	byte _keyStates[256];

	Hashmap<EKeycode, KeyBind*> _keyBinds;
	Hashmap<EAxis, float*> _axisBinds;

public:
	InputManager() : _keyStates() {}
	~InputManager();

	void BindAxis(EAxis axis, float *axisPtr) { _axisBinds.Set(axis, axisPtr); }

	void BindKey(EKeycode key, const Callback &callback) { _keyBinds.Set(key, new KeyBind_Callback(callback)); }
	void BindKeyAxis(EKeycode key, float *axisPtr, float axisDisplacement) { _keyBinds.Set(key, new KeyBind_Axis(axisPtr, axisDisplacement)); }

	bool IsKeyDown(EKeycode key) const { return _keyStates[(byte)key] == 1; }

	void KeyDown(EKeycode);
	void KeyUp(EKeycode);

	void Reset();

	void ClearMouseInput();

	//xDelta and yDelta are in pixels
	void AddMouseInput(short xDelta, short yDelta);
};
