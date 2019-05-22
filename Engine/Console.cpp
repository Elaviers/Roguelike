#include "Console.hpp"
#include "GLProgram.hpp"
#include "Transform.hpp"

const int maxAllocSize = 2048;
const int allocUnitSize = 128;

const int linesToDraw = 15;

void Console::_CMD_echo(const Buffer<String>& tokens)
{
	if (tokens.GetSize() > 0)
		Print(CSTR(tokens[0] + '\n'));
}

void Console::_SetNextChar(char c)
{
	if (_nextBufferIndex >= _charBuffer.GetSize())
		_ExpandBuffer();

	_charBuffer[_nextBufferIndex] = c;
}

void Console::_ExpandBuffer()
{
	if (maxAllocSize && _charBuffer.GetSize() + allocUnitSize >= maxAllocSize)
	{
		Utilities::CopyBytes(&_charBuffer[allocUnitSize], &_charBuffer[0], _charBuffer.GetSize() - allocUnitSize);
		_nextBufferIndex -= maxAllocSize;
	}
	else
		_charBuffer.Append(allocUnitSize);
}

void Console::Print(const char *string)
{
	for (int i = 0; string[i] != '\0'; ++i)
	{
		_SetNextChar(string[i]);

		++_nextBufferIndex;
	}

	_SetNextChar('\0');
}

void Console::Render(const Font &font, float deltaTime)
{
	static const float timerTarget = .5f;

	_blinkTimer += deltaTime;

	while (_blinkTimer >= timerTarget)
	{
		_blinkTimer -= timerTarget;
		_showCursor = !_showCursor;
	}

	int linesNeeded = 0;

	float yOffset = 10;

	Transform transform;
	transform.SetScale(Vector3(32, 32, 1));

	if (_charBuffer.GetSize())
	{
		size_t startIndex = 0;

		{
			size_t i = _nextBufferIndex - 1;
			size_t lc = 0;

			while (true)
			{
				if (_charBuffer[i] == '\n')
					++lc;

				if (lc >= linesToDraw)
				{
					startIndex = i;
					break;
				}

				if (i == 0)
					break;

				--i;
			}

			linesNeeded = Utilities::Min((int)lc, linesToDraw);
		}

		transform.SetPosition(Vector3(0, yOffset + linesNeeded * transform.GetScale()[1], 0));

		font.RenderString(&_charBuffer[startIndex], transform, transform.GetScale()[1]);
	}

	transform.SetPosition(Vector3(0, 0, 0));

	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, .2f, 0.f));
	font.RenderString(&_prePrompt[0], transform);
	transform.Move(Vector3(font.CalculateStringWidth(&_prePrompt[0], transform.GetScale()[0]), 0.f, 0.f));

	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Vector4(0.2f, 1.f, 0.2f));
	font.RenderString(&_prompt[0], transform);
	transform.Move(Vector3(font.CalculateStringWidth(&_prompt[0], transform.GetScale()[0]), 0.f, 0.f));

	if (_showCursor)
		font.RenderString("|", transform);
}
