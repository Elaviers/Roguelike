#include "Font_TTF.hpp"
#include "Colour.hpp"
#include "FontManager.hpp"
#include "RenderCommand.hpp"
#include "RenderQueue.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/MacroUtilities.hpp>
#include <ELMaths/Transform.hpp>

const PropertyCollection& FontTTF::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.AddCommand("filename", MemberCommandPtr<FontTTF>(&FontTTF::_CMD_LoadFont));
	properties.Add<int>("rendersize", offsetof(FontTTF, _size));
	DO_ONCE_END;

	return properties;
}

void FontTTF::_CMD_LoadFont(const Buffer<String>& args, const Context& ctx)
{
	if (args.GetSize() > 0)
	{
		FontManager* fontManager = ctx.GetPtr<FontManager>();
		FT_Library* ftLib = ctx.GetPtr<FT_Library>();
		if (fontManager && ftLib)
		{
			String fp = args[0];

			const Buffer<String> paths = fontManager->GetPaths();
			for (size_t i = 0; i < paths.GetSize(); ++i)
			{
				fp = paths[i] + args[0];

				if (IO::FileExists(fp.GetData()))
					break;
			}

			FT_Error error = FT_New_Face(*ftLib, fp.GetData(), 0, &_face);
			if (error)
			{
				Debug::Error(CSTR("Could not load font file \"", fp, '\"'));
				return;
			}

			error = FT_Set_Pixel_Sizes(_face, 0, _size);
			if (error)
				Debug::Error("FT_Set_Pixel_Sizes error");

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			for (int i = 0; i < 128; ++i)
			{
				error = FT_Load_Char(_face, i, FT_LOAD_RENDER);

				if (error)
				{
					Debug::PrintLine(CSTR("COULD NOT LOAD CHAR \'", (char)i, '\''));
					continue;
				}

				GLuint texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
					_face->glyph->bitmap.width, _face->glyph->bitmap.rows, 0,
					GL_RED, GL_UNSIGNED_BYTE, _face->glyph->bitmap.buffer);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				TTFGlyph glyph = {
					texture,
					Vector2((float)_face->glyph->bitmap.width, (float)_face->glyph->bitmap.rows),
					Vector2((float)_face->glyph->bitmap_left, (float)_face->glyph->bitmap_top),
					_face->glyph->advance.x
				};

				_charMap[(char)i] = glyph;
			}

			_descender = _face->descender >> 6;

			FT_Done_Face(_face);
		}
	}
}

float FontTTF::CalculateStringWidth(const char* string, float scaleX, size_t maxChars) const
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
			const TTFGlyph* space = _charMap.Get(' ');
			float stopWidth = (space->advance >> 6) * scale * 5.f;
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
			const TTFGlyph* glyph = _charMap.Get(*c);
			if (glyph)
				currentLineW += (glyph->advance >> 6) * scale;
		}

		if (++i == maxChars) break;
	}

	return Maths::Max(longestWidth, currentLineW);
}

size_t FontTTF::GetPositionOf(float pX, float pY, const char* string, const Transform& transform, float lineHeight) const
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
			const TTFGlyph* space = _charMap.Get(' ');
			float stopWidth = (space->advance >> 6) * scale * 5.f;
			float nextStop = Maths::Trunc(currentLineW, stopWidth) + stopWidth;
			currentCharW = (nextStop - currentLineW);
		}
		else
		{
			const TTFGlyph* glyph = _charMap.Get(*c);
			if (glyph)
			{
				currentCharW = (glyph->advance >> 6) * scale;
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

void FontTTF::RenderString(RenderEntry& e, const char* string, const Transform& transform, float lineHeight) const
{
	Transform t;
	Vector3 advanceDirection = t.GetRightVector();
	Vector3 upDirection = t.GetUpVector();
	Vector3 downDirection = -1.f * upDirection;

	t.SetPosition(transform.GetPosition());

	float scale = (transform.GetScale().x / (float)_size);

	float line = 0.f;
	float currentLineW = 0.f;

	float yOffset = (float)_descender;

	t.Move(downDirection * yOffset);

	e.AddSetIsFont(true);
	e.AddCommand(RCMDSetUVScale::Default());
	e.AddCommand(RCMDSetUVOffset::Default());

	for (const char* c = string; *c != '\0'; ++c)
	{
		if (*c == 0x01)
		{
			e.AddSetColour(Colour::FromColourCode(c));

			c += 4;
		}
		else if (*c == '\n' && lineHeight)
		{
			++line;
			currentLineW = 0.f;

			t = transform;
			t.Move(downDirection * (yOffset + lineHeight * line));
		}
		else if (*c == '\t')
		{
			const TTFGlyph* space = _charMap.Get(' ');
			float stopWidth = (space->advance >> 6) * scale * 5.f;
			float nextStop = Maths::Trunc(currentLineW, stopWidth) + stopWidth;

			t.Move(advanceDirection * (nextStop - currentLineW));
			currentLineW = nextStop;
		}
		else
		{
			const TTFGlyph* glyph = _charMap.Get(*c);
			if (glyph)
			{
				t.SetScale(Vector3(glyph->size.x * scale, glyph->size.y * scale, 1.f));

				Vector3 v = t.GetPosition() + (advanceDirection * (glyph->bearing.x + glyph->size.x / 2.f) * scale) + (downDirection * (glyph->size.y / 2.f - glyph->bearing.y) * scale);

				e.AddSetTransform(Matrix4::Transformation(v, t.GetRotation().GetQuat(), t.GetScale()));
				e.AddSetTextureGL(glyph->texID, 0);
				e.AddCommand(RCMDRenderMesh::PLANE);

				t.Move(advanceDirection * (float)(glyph->advance >> 6) * scale);
				currentLineW += (float)(glyph->advance >> 6) * scale;
			}
		}
	}

	e.AddSetIsFont(false);
}
