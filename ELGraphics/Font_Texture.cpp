#include "Font_Texture.hpp"
#include "Colour.hpp"
#include "RenderCommand.hpp"
#include "RenderEntry.hpp"
#include "TextureManager.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/MacroUtilities.hpp>

const PropertyCollection& FontTexture::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.AddCommand("texture", MemberCommandPtr<FontTexture>(&FontTexture::_CMD_texture));
	properties.AddCommand("region", MemberCommandPtr<FontTexture>(&FontTexture::_CMD_region));
	properties.Add<int>("size", offsetof(FontTexture, _size));
	properties.Add<int>("row_h", offsetof(FontTexture, _rowH));
	properties.Add<int>("y_offset", offsetof(FontTexture, _yOffset));
	DO_ONCE_END;

	return properties;
}

void FontTexture::_CMD_texture(const Buffer<String>& args, const Context& ctx)
{
	if (args.GetSize() > 0)
	{
		TextureManager* textureManager = ctx.GetPtr<TextureManager>();
		if (textureManager) textureManager->Get(args[0], ctx);
	}
}

void FontTexture::_CMD_region(const Buffer<String>& args, const Context& ctx)
{
	if (args.GetSize() >= 6)
	{
		if (args[0] == "space")
		{
			Glyph& glyph = _charMap[' '];
			glyph.uv.x = (float)args[1].ToInt();
			glyph.uv.y = (float)args[2].ToInt();
			glyph.uvSize.x = (float)args[3].ToInt();
			glyph.uvSize.y = (float)args[4].ToInt();
			glyph.advance = args[5].ToInt();

			glyph.width = (int)(glyph.uvSize.x * _texture->GetWidth());
			if (glyph.width == 0)
				glyph.width = glyph.advance;
		}
	}
}

void FontTexture::_ReadText(const String & string, const Context& ctx)
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

					glyph.uv.x = cX / (float)_texture->GetWidth();
					glyph.uv.y = cY / (float)_texture->GetHeight();
					glyph.uvSize.x = glyph.width / (float)_texture->GetWidth();
					glyph.uvSize.y = _rowH / (float)_texture->GetHeight();

					cX += glyph.width;
				}
			}

			break;
		}

		String unused = GetProperties().HandleCommand(this, loweredLine, ctx);
	}
}

float FontTexture::CalculateStringWidth(const char* string, float scaleX, size_t maxChars) const
{
	float longestWidth = 0.f;
	float currentLineW = 0.f;

	float scale = scaleX / (float)_size;
	size_t i = 0;
	for (const char* c = string; *c != '\0'; ++c)
	{
		if (*c == 0x01)
		{
			c += 4;
		}
		else if (*c == '\t')
		{
			const Glyph* space = _charMap.Get(' ');
			float stopWidth = space->width * scale * 5.f;
			float nextStop = Maths::Trunc(currentLineW, stopWidth) + stopWidth;

			currentLineW = nextStop;
		}
		else if (*c == '\n')
		{
			if (currentLineW > longestWidth)
				longestWidth = currentLineW;

			currentLineW = 0.f;
		}
		else
		{
			const Glyph* glyph = _charMap.Get(*c);
			if (glyph)
				currentLineW += (glyph->width + glyph->advance) * scale;
		}

		if (++i == maxChars) break;
	}

	return Maths::Max(longestWidth, currentLineW);
}

size_t FontTexture::GetPositionOf(float pX, float pY, const char* string, const Transform& transform, float lineHeight) const
{
	Vector3 advanceDirection = transform.GetRightVector();
	Vector3 downDirection = -1.f * transform.GetUpVector();

	float x = transform.GetPosition().x;
	float y = transform.GetPosition().y;

	float scale = (transform.GetScale().x / (float)_size);

	float line = 0.f;
	float currentLineW = 0.f;
	float currentCharW = 0.f;

	size_t i = 0;
	for (const char* c = string; *c != '\0'; ++c)
	{
		if (*c == 0x01)
		{
			c += 4;
		}
		else if (*c == '\n' && lineHeight)
		{
			++line;
			currentLineW = 0.f;
			currentCharW = 0.f;

			float x = transform.GetPosition().x;
			float y = transform.GetPosition().y;
			x += downDirection.x * lineHeight * line;
			y += downDirection.y * lineHeight * line;
		}
		else if (*c == '\t')
		{
			const Glyph* space = _charMap.Get(' ');
			float stopWidth = space->width * scale * 5.f;
			float nextStop = Maths::Trunc(currentLineW, stopWidth) + stopWidth;
			currentCharW = (nextStop - currentLineW);
		}
		else
		{
			const Glyph* glyph = _charMap.Get(*c);
			if (glyph)
			{
				currentCharW = (glyph->width + glyph->advance) * scale;
			}
		}

		if (x + advanceDirection.x * currentCharW >= pX)
		{
			if (x + advanceDirection.x * currentCharW / 2.f <= pX)
				return i + 1;

			return i;
		}

		x += advanceDirection.x * currentCharW;
		y += advanceDirection.y * currentCharW;
		currentLineW += currentCharW;

		++i;
	}

	return StringLength(string);
}

void FontTexture::RenderString(RenderEntry& e, const char* string, const Transform & transform, float lineHeight) const
{
	Transform charTransform = transform;
	Vector3 advanceDirection = charTransform.GetRightVector();
	Vector3 downDirection = -1.f * charTransform.GetUpVector();

	charTransform.Move(Vector3(0.f, Maths::Round(transform.GetScale().y / 2.f), 0.f));

	float scale = (transform.GetScale().x / (float)_size);

	float line = 0.f;
	float currentLineW = 0.f;

	const Vector2 halfTexel(.5f / ((float)_texture->GetWidth() * scale), .5f / ((float)_texture->GetHeight() * scale));
	const Vector2 texel(1.f / ((float)_texture->GetWidth() * scale), 1.f / ((float)_texture->GetHeight() * scale));

	Colour colour = Colour::White;
	
	bool firstGlyph = true;

	for (const char* c = string; *c != '\0'; ++c)
	{
		if (*c == 0x01)
		{
			//Colour code
			colour = Colour::FromColourCode(c).GetData();
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
				charTransform.SetScale(Vector3(glyph->width * scale, (float)transform.GetScale().y, 1.f));

				e.AddSetTransform(charTransform.MakeTransformationMatrix());
				e.AddSetUVOffset(glyph->uv + halfTexel);
				e.AddSetUVScale(glyph->uvSize - texel);
				e.AddSetTexture(*_texture, 0);
				e.AddCommand(RCMDRenderMesh::PLANE);
				
				float secondHalfWPlusAdvance = (((glyph->width / 2.f) + glyph->advance) * scale);
				charTransform.Move(advanceDirection * secondHalfWPlusAdvance);

				currentLineW += (glyph->width + glyph->advance) * scale;
				firstGlyph = false;
			}
		}
	}
}

