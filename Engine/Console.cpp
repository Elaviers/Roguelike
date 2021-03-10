#include "Console.hpp"
#include <ELCore/Context.hpp>
#include <ELGraphics/Colour.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELMaths/Transform.hpp>

const int maxAllocSize = 2048;
const int allocUnitSize = 128;

const int linesToDraw = 128;

void Console::_CMD_echo(const Buffer<String>& tokens, const Context&)
{
	if (tokens.GetSize() > 0)
		Print(CSTR(Colour::Cyan.ToColourCode(), tokens[0], '\n'));
}

void Console::_CMD_help(const Buffer<String>& tokens, const Context&)
{
	auto all = _cvars.GetAll();

	for (int i = 0; i < all.GetSize(); ++i)
	{
		Print(CSTR(
			Colour::Green.ToColourCode(), all[i]->GetName(), "\t\t",
			Colour::Yellow.ToColourCode(), all[i]->GetTypeName(), '\n'));
	}
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
		_charBuffer.Grow(allocUnitSize);
}

void Console::Print(const char* string)
{
	for (int i = 0; string[i] != '\0'; ++i)
	{
		_SetNextChar(string[i]);

		++_nextBufferIndex;
	}

	_SetNextChar('\0');
}

void Console::InputChar(char key, const Context& ctx)
{
	if (key == '\b')
		Backspace();
	else if (key == '\r')
		SubmitPrompt(ctx);
	else
	{
		_prompt += key;
		_ResetBlink();
	}
}


void Console::Backspace()
{
	if (_prompt.GetLength())
	{
		_prompt.Shrink(1);
		_ResetBlink();
	}
}

void Console::SubmitPrompt(const Context& ctx)
{
	Print(CSTR(Colour::White.ToColourCode(), _prePrompt, _prompt, Colour::Grey.ToColourCode()));

	String result = _cvars.HandleCommand(_prompt.Split(" "), ctx);
	
	if (result.GetLength())
		Print(result.GetData());

	_prompt.Clear();
	_ResetBlink();
}

void Console::Render(RenderQueue& q, const Font& font, float deltaTime)
{
	static const float timerTarget = .5f;

	_blinkTimer += deltaTime;

	while (_blinkTimer >= timerTarget)
	{
		_blinkTimer -= timerTarget;
		_showCursor = !_showCursor;
	}

	int linesNeeded = 0;

	float yOffset = 0;

	Transform transform;
	transform.SetScale(Vector3(16, 16, 1));

	RenderEntry& e = q.CreateEntry(ERenderChannels::UNLIT);

	if (_charBuffer.GetSize())
	{
		size_t startIndex = 0;

		{
			size_t i = _nextBufferIndex - 1;
			size_t lc = 1;

			while (true)
			{
				if (_charBuffer[i] == '\n' && _charBuffer[i + 1] != '\0')
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

			linesNeeded = Maths::Min((int)lc, linesToDraw);
		}

		transform.SetPosition(Vector3(0, yOffset + linesNeeded * transform.GetScale().y, 0));

		font.RenderString(e, &_charBuffer[startIndex], transform, transform.GetScale().y);
	}

	transform.SetPosition(Vector3(0, 0, 0));
	e.AddSetColour(Colour(1.f, .2f, 0.f));

	font.RenderString(e, &_prePrompt[0], transform);
	transform.Move(Vector3(font.CalculateStringWidth(&_prePrompt[0], transform.GetScale().x), 0.f, 0.f));

	e.AddSetColour(Colour(.2f, 1.f, .2f));
	font.RenderString(e, &_prompt[0], transform);
	transform.Move(Vector3(font.CalculateStringWidth(&_prompt[0], transform.GetScale().x), 0.f, 0.f));

	if (_showCursor)
		font.RenderString(e, "|", transform);
}

///////////////
#include <ELGraphics/MaterialManager.hpp>
#include <ELGraphics/TextureManager.hpp>

constexpr const char* MAXANISOTROPY = "max_aniso";
constexpr const char* MAXMIPS = "max_mip_levels";

void Console::CMD_texmgr(const Buffer<String>& args, const Context& ctx)
{
	TextureManager* textureManager = ctx.GetPtr<TextureManager>();
	MaterialManager* materialManager = ctx.GetPtr<MaterialManager>();

	if (args.GetSize() >= 2)
	{
		String var = args[0].ToLower();

		if (var == MAXANISOTROPY)
		{
			textureManager->SetMaxAnisotropy(args[1].ToInt());
			textureManager->Refresh(ctx);
			materialManager->Refresh(ctx);
			return;
		}

		if (var == MAXMIPS)
		{
			textureManager->SetMaxMipLevels(args[1].ToInt());
			textureManager->Refresh(ctx);
			materialManager->Refresh(ctx);
			return;
		}
	}

	Print(CSTR(MAXANISOTROPY, '=', (int)textureManager->GetMaxAnisotropy(), '\n'));
	Print(CSTR(MAXMIPS, '=', (int)textureManager->GetMaxMipLevels(), '\n'));
}
