#include "Font.h"
#include "Engine.h"
#include "GLProgram.h"
#include "TextureManager.h"
#include "ModelManager.h"

void Font::_CMD_texture(const Buffer<String>& args)
{
	if (args.GetSize() > 0)
		_texture = Engine::Instance().pTextureManager->Get(args[0]);
}

void Font::_CMD_region(const Buffer<String> &args)
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

void Font::FromString(const String &string)
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
					Glyph &glyph = _charMap[tokens[0][0]];
					glyph.width = tokens[1].ToInt();
					glyph.advance = tokens[2].ToInt();

					if (cX + glyph.width >= _texture->GetWidth())
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

		_cvars.HandleCommand(loweredLine);
	}
}

float Font::CalculateStringWidth(const char *string, float scaleX) const
{
	float width = 0.f;

	for (const char *c = string; *c != '\0'; ++c)
	{
		const Glyph *glyph = _charMap.Find(*c);
		if (glyph)
			width += (glyph->width + glyph->advance) * (scaleX / (float)_size);
	}

	return width;
}

void Font::RenderString(const char *string, const Transform &_transform, float lineHeight) const
{
	_texture->Bind(0);

	Transform charTransform = _transform;
	Vector3 advanceDirection = charTransform.GetRightVector();
	Vector3 downDirection = -1.f * charTransform.GetUpVector();

	charTransform.Move(Vector3(0.f, Maths::Round(_transform.GetScale()[1] / 2.f), 0.f));

	float scale = (_transform.GetScale()[0] / (float)_size);

	float totalAdvance = 0.f;
	
	for (const char *c = string; *c != '\0'; ++c)
	{
		const Vector2 halfTexel(.5f / ((float)_texture->GetWidth() * scale), .5f / ((float)_texture->GetHeight() * scale));
		const Vector2 texel(1.f / ((float)_texture->GetWidth() * scale), 1.f / ((float)_texture->GetHeight() * scale));

		const Glyph *glyph = _charMap.Find(*c);
		if (glyph)
		{
			float halfCharW = (glyph->width * scale / 2.f);
			charTransform.Move(advanceDirection * halfCharW);
			charTransform.SetScale(Vector3(glyph->width * scale, (float)_transform.GetScale()[1], 1.f));

			GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVOffset, glyph->uvOffset + halfTexel);
			GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, glyph->uvSize - texel);
			GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, charTransform.MakeTransformationMatrix());
			Engine::Instance().pModelManager->Plane().Render();

			float secondHalfWPlusAdvance = (((glyph->width / 2.f) + glyph->advance) * scale);
			charTransform.Move(advanceDirection * secondHalfWPlusAdvance);

			totalAdvance += halfCharW + secondHalfWPlusAdvance;
		}

		if (*c == '\n')
		{
			charTransform.Move(advanceDirection * -totalAdvance);
			charTransform.Move(downDirection * lineHeight);

			totalAdvance = 0;
		}
	}
}
