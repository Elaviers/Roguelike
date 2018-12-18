#include "Font.h"
#include "Engine.h"
#include "GLProgram.h"

Font::Font()
{
}


Font::~Font()
{
}

void Font::_HandleCommand(const String &string)
{
	Buffer<String> tokens = string.Split(" ");

	if (tokens[0] == "region")
	{
		if (tokens.GetSize() >= 7)
		{
			if (tokens[1] == "SPACE")
			{
				Glyph &glyph = _charMap[' '];
				glyph.uvOffset[0] = tokens[2].ToInt();
				glyph.uvOffset[1] = tokens[3].ToInt();
				glyph.uvSize[0] = tokens[4].ToInt();
				glyph.uvSize[1] = tokens[5].ToInt();
				glyph.advance = tokens[6].ToInt();

				glyph.width = glyph.uvSize[0] * _texture->GetWidth();
				if (glyph.width == 0)
					glyph.width = glyph.advance;
			}
		}
	}
	else
		Engine::materialManager->HandleCommand(string);

}

void Font::FromString(const String &string)
{
	Buffer<String> lines = string.Split("\r\n");
	
	for (uint32 i = 0; i < lines.GetSize(); ++i)
	{
		if (lines[i] != "GLYPHS")
		{
			Buffer<String> tokens = lines[i].Split("=");

			if (tokens.GetSize() > 1)
			{
				if (tokens[0] == "texture")
					_texture = Engine::textureManager->GetTexture(tokens[1]);
				else if (tokens[0] == "y_offset")
					_yOffset = tokens[1].ToInt();
				else if (tokens[0] == "divs_x")
					_divsX = tokens[1].ToInt();
				else if (tokens[0] == "divs_y")
					_divsY = tokens[1].ToInt();
			}
			else _HandleCommand(lines[i]);
		}
		else
		{
			if (!_texture) return;

			i++;
			for (uint32 slot = 0; i + slot < lines.GetSize(); ++slot)
			{
				Buffer<String> tokens = lines[i + slot].Split(" ");

				float glyphUVW = 1.f / _divsX;
				float glyphUVH = -1.f / _divsY;

				if (tokens.GetSize() > 1)
				{
					Glyph &glyph = _charMap[tokens[0][0]];
					glyph.width = glyphUVW * _texture->GetWidth();
					glyph.advance = tokens[1].ToInt();

					glyph.uvOffset[0] = (slot % _divsX) / (float)_divsX;
					glyph.uvOffset[1] = ((slot / _divsX) + 1) / (float)_divsY;
					glyph.uvSize[0] = glyphUVW;
					glyph.uvSize[1] = glyphUVH;
				}
			}

			break;
		}
	}
}

float Font::CalculateStringWidth(const char *string, float scaleX) const
{
	float width = 0.f;

	for (const char *c = string; *c != '\0'; ++c)
	{
		const Glyph *glyph = _charMap.Find(*c);
		if (glyph)
			width += (float)glyph->advance / (float)glyph->width * scaleX;
	}

	return width;
}

void Font::RenderString(const char *string, const Transform &transform) const
{
	_texture->Bind(0);

	Transform charTransform = transform;
	Vector3 advanceDirection = charTransform.GetRightVector();

	charTransform.Move(Vector3(transform.Scale()[0] / 2.f, transform.Scale()[1] / 2.f, 0.f));

	for (const char *c = string; *c != '\0'; ++c)
	{
		const Glyph *glyph = _charMap.Find(*c);
		if (glyph)
		{
			GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVOffset, glyph->uvOffset);
			GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, glyph->uvSize);
			GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, charTransform.MakeTransformationMatrix());
			Engine::modelManager->Plane().Render();

			if (glyph->width)
				charTransform.Move(advanceDirection * (float)glyph->advance / (float)glyph->width * transform.Scale()[0]);
			else
				charTransform.Move(advanceDirection * (float)glyph->advance * transform.Scale()[0]);
		}
	}
}
