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

	inline void _ResetBlink()
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
		_cvars.CreateVar("echo", CommandPtr(this, &Console::_CMD_echo));
		_cvars.CreateVar("help", CommandPtr(this, &Console::_CMD_help));
	}

	~Console() {}

	inline RigidPropertyCollection& Cvars() { return _cvars; }

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
