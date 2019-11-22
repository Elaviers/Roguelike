#pragma once
#include "Buffer.hpp"
#include "RigidPropertyCollection.hpp"
#include "Font.hpp"
#include <Windows.h>

class Console
{
	RigidPropertyCollection _cvars;

	Buffer<char> _charBuffer;

	String _prePrompt;
	String _prompt;

	size_t _nextBufferIndex;

	bool _showCursor = true;
	float _blinkTimer = 0.f;

	void _ResetBlink()
	{
		_showCursor = true;
		_blinkTimer = 0.f;
	}

	void _SetNextChar(char c);

	void _ExpandBuffer();

	void _CMD_echo(const Buffer<String>& tokens);
	void _CMD_help(const Buffer<String>& tokens);

public:
	Console() : _nextBufferIndex(0), _prePrompt('>') 
	{
		_cvars.CreateVar("Echo", CommandPtr(this, &Console::_CMD_echo));
		_cvars.CreateVar("Help", CommandPtr(this, &Console::_CMD_help));
	}

	~Console() {}

	RigidPropertyCollection& Cvars() { return _cvars; }

	void Print(const char* string);
	void InputChar(char key);
	void Backspace();
	void SubmitPrompt();
	void Render(const Font &font, float deltaTime);
};
