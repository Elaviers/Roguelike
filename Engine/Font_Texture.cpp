#include "Font_Texture.hpp"
#include "Colour.hpp"
#include "Engine.hpp"
#include "GLProgram.hpp"
#include "MacroUtilities.hpp"
#include "ModelManager.hpp"

const PropertyCollection& FontTexture::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.Add("mag", MemberGetter<FontTexture, String>(&FontTexture::_GetMag), MemberSetter<FontTexture, String>(&FontTexture::_SetMag));
	properties.AddCommand("texture", MemberCommandPtr<FontTexture>(&FontTexture::_CMD_texture));
	properties.AddCommand("region", MemberCommandPtr<FontTexture>(&FontTexture::_CMD_region));
	properties.Add<int>("size", offsetof(FontTexture, _size));
	properties.Add<int>("row_h", offsetof(FontTexture, _rowH));
	properties.Add<int>("y_offset", offsetof(FontTexture, _yOffset));
	properties.Add<byte>("mips", offsetof(FontTexture, _mips));
	DO_ONCE_END;

	return properties;
}

void FontTexture::_SetMag(const String& mag)
{
	
}

void FontTexture::_CMD_texture(const Buffer<String>& args)
{
	if (args.GetSize() > 0)
		_texture = Engine::Instance().pTextureManager->Get(args[0]);
}

void FontTexture::_CMD_region(const Buffer<String>& args)
{
	if (args.GetSize() >= 6)
	{
		if (args[0] == "space")
		{
			Glyph& glyph = _charMap[' '];
			glyph.uvOffset[0] = (float)args[1].ToInt();
			glyph.uvOffset[1] = (float)args[2].ToInt();
			glyph.uvSize[0] = (float)args[3].ToInt();
			glyph.uvSize[1] = (float)args[4].ToInt();
			glyph.advance = args[5].ToInt();

			glyph.width = (int)(glyph.uvSize[0] * _texture->GetWidth());
			if (glyph.width == 0)
				glyph.width = glyph.advance;
		}
	}
}

void FontTexture::_ReadText(const String & string)
{
	Buffer<String> lines = string.Split("\r\n");

	for (uint32 i = 0; i < lines.GetSize(); ++i)
	{
		String loweredLine = lines[i].ToLower();

		if (loweredLine == "glyphs")
		{
			if (!_texture) return;

			int cX = 0;
			int cY = 0;

			i++;
			for (size_t slot = 0; i + slot < lines.GetSize(); ++slot)
			{
				Buffer<String> tokens = lines[i + slot].Split(" ");

				float glyphUVH = -_rowH / (float)_texture->GetHeight();

				if (tokens.GetSize() > 1)
				{
					Glyph& glyph = _charMap[tokens[0][0]];
					glyph.width = tokens[1].ToInt();
					glyph.advance = tokens[2].ToInt();

					if (cX + glyph.width >= (int)_texture->GetWidth())
					{
						cX = 0;
						cY += _rowH;
					}

					glyph.uvOffset[0] = cX / (float)_texture->GetWidth();
					glyph.uvOffset[1] = (cY + _rowH) / (float)_texture->GetHeight();
					glyph.uvSize[0] = glyph.width / (float)_texture->GetWidth();
					glyph.uvSize[1] = glyphUVH;

					cX += glyph.width;
				}
			}

			break;
		}

		String unused = GetProperties().HandleCommand(this, loweredLine);
	}
}

float FontTexture::CalculateStringWidth(const char* string, float scaleX) const
{
	float width = 0.f;

	float scale = scaleX / (float)_size;

	for (const char* c = string; *c != '\0'; ++c)
	{
		if (*c == '\t')
		{
			const Glyph* space = _charMap.Get(' ');
			float stopWidth = space->width * scale * 5.f;
			float nextStop = Maths::Trunc(width, stopWidth) + stopWidth;

			width = nextStop;
		}
		else
		{
			const Glyph* glyph = _charMap.Get(*c);
			if (glyph)
				width += (glyph->width + glyph->advance) * scale;
		}
	}

	return width;
}

void FontTexture::RenderString(const char* string, const Transform & transform, float lineHeight) const
{
	_texture->Bind(0);

	Transform charTransform = transform;
	Vector3 advanceDirection = charTransform.GetRightVector();
	Vector3 downDirection = -1.f * charTransform.GetUpVector();

	charTransform.Move(Vector3(0.f, Maths::Round(transform.GetScale()[1] / 2.f), 0.f));

	float scale = (transform.GetScale()[0] / (float)_size);

	float line = 0.f;
	float currentLineW = 0.f;

	const Vector2 halfTexel(.5f / ((float)_texture->GetWidth() * scale), .5f / ((float)_texture->GetHeight() * scale));
	const Vector2 texel(1.f / ((float)_texture->GetWidth() * scale), 1.f / ((float)_texture->GetHeight() * scale));

	for (const char* c = string; *c != '\0'; ++c)
	{
		if (*c == 0x01)
		{
			//Colour code
			GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Colour::FromColourCode(c).GetData());
			c += 4;
		}
		else if (*c == '\n' && lineHeight)
		{
			++line;
			currentLineW = 0.f;

			charTransform = transform;
			charTransform.Move(downDirection * lineHeight * line);
		}
		else if (*c == '\t')
		{
			const Glyph* space = _charMap.Get(' ');
			float stopWidth = space->width * scale * 5.f;
			float nextStop = Maths::Trunc(currentLineW, stopWidth) + stopWidth;

			charTransform.Move(advanceDirection * (nextStop - currentLineW));
			currentLineW = nextStop;
		}
		else
		{
			const Glyph* glyph = _charMap.Get(*c);
			if (glyph)
			{
				float halfCharW = (glyph->width * scale / 2.f);
				charTransform.Move(advanceDirection * halfCharW);
				charTransform.SetScale(Vector3(glyph->width * scale, (float)transform.GetScale()[1], 1.f));

				GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVOffset, glyph->uvOffset + halfTexel);
				GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, glyph->uvSize - texel);
				GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, charTransform.MakeTransformationMatrix());
				Engine::Instance().pModelManager->Plane().Render();

				float secondHalfWPlusAdvance = (((glyph->width / 2.f) + glyph->advance) * scale);
				charTransform.Move(advanceDirection * secondHalfWPlusAdvance);

				currentLineW += (glyph->width + glyph->advance) * scale;
			}
		}
	}
}

