#pragma once
#include <ELCore/Buffer.hpp>
#include <ELCore/RigidPropertyCollection.hpp>
#include <ELGraphics/Font.hpp>
#include <Windows.h>

class RenderQueue;

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

	void _CMD_echo(const Buffer<String>& tokens, const Context&);
	void _CMD_help(const Buffer<String>& tokens, const Context&);


public:
	Console() : _nextBufferIndex(0), _prePrompt('>')
	{
		_cvars.CreateVar("Echo", CommandPtr(*this, &Console::_CMD_echo));
		_cvars.CreateVar("Help", CommandPtr(*this, &Console::_CMD_help));
	}

	~Console() {}

	RigidPropertyCollection& Cvars() { return _cvars; }

	void Print(const char* string);
	void InputChar(char key, const Context& ctx);
	void Backspace();
	void SubmitPrompt(const Context& ctx);

	void Render(RenderQueue& q, const Font& font, float deltaTime);
	
	void CMD_texmgr(const Buffer<String>& tokens, const Context&);
};
