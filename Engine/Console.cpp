#include "Console.h"
#include "Transform.h"

const int maxAllocSize = 2048;
const int allocUnitSize = 128;

const int linesToDraw = 5;

void Console::_CMD_echo(const Buffer<String>& tokens)
{
	if (tokens.GetSize() > 0)
		Print(CSTR(tokens[0] + '\n'));
}

void Console::_ExpandBuffer()
{
	if (maxAllocSize && _charBuffer.GetSize() + allocUnitSize >= maxAllocSize)
	{
		Utilities::CopyBytes(&_charBuffer[allocUnitSize], &_charBuffer[0], _charBuffer.GetSize() - allocUnitSize);
		_endOfString -= maxAllocSize;
	}
	else
		_charBuffer.Append(allocUnitSize);
}

void Console::Print(const char *string)
{
	for (int i = 0; string[i] != '\0'; ++i)
	{
		++_endOfString;

		if (_endOfString > _charBuffer.GetSize())
			_ExpandBuffer();

		_charBuffer[_endOfString] = string[i];
	}
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

	Transform transform;
	transform.SetScale(Vector3(128, 128, 1));

	if (_charBuffer.GetSize())
	{
		size_t startIndex = 0;

		{
			size_t i = _charBuffer.GetSize() - 1;
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

		transform.SetPosition(Vector3(0, linesNeeded * transform.GetScale()[1], 0));

		font.RenderString(&_charBuffer[startIndex], transform, transform.GetScale()[1]);
	}

	transform.SetPosition(Vector3(0, 0, 0));

	if (_prompt.GetLength())
	{
		font.RenderString(&_prompt[0], transform);

		transform.Move(Vector3(font.CalculateStringWidth(&_prompt[0], transform.GetScale()[0]), 0.f, 0.f));
	}

	if (_showCursor)
		font.RenderString("l", transform);
}
