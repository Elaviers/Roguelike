#pragma once
#include "Buffer.hpp"
#include "CvarMap.hpp"
#include "Font.hpp"
#include <Windows.h>

class Console
{
	CvarMap _cvars;

	Buffer<char> _charBuffer;

	String _prePrompt;
	String _prompt;

	size_t _nextBufferIndex;

	bool _showCursor = true;
	float _blinkTimer = 0.f;

	inline void _ResetBlink()
	{
		_showCursor = true;
		_blinkTimer = 0.f;
	}

	void _CMD_echo(const Buffer<String>& tokens);

	void _SetNextChar(char c);

	void _ExpandBuffer();

public:
	Console() : _nextBufferIndex(0), _prePrompt('>') {}
	~Console() {}

	inline CvarMap& Cvars() { return _cvars; }

	inline void Initialise() { _cvars.CreateVar("echo", CommandPtr(this, &Console::_CMD_echo)); }

	void Print(const char* string);

	inline void InputChar(char key) 
	{
		if (key == '\b')
			Backspace();
		else if (key == '\r')
			SubmitPrompt();
		else
		{
			_prompt += key;
			_ResetBlink();
		}
	}


	inline void Backspace()
	{
		if (_prompt.GetLength())
		{
			_prompt.Shrink(1);
			_ResetBlink();
		}
	}

	inline void SubmitPrompt() 
	{ 
		Print((_prePrompt + _prompt + '\n').GetData());
		Print(_cvars.HandleCommand(_prompt).GetData());
		_prompt.Clear(); 
		_ResetBlink();
	}

	void Render(const Font &font, float deltaTime);
};
