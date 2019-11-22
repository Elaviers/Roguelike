#include "Font_TTF.hpp"
#include "Colour.hpp"
#include "Engine.hpp"
#include "GLProgram.hpp"
#include "FontManager.hpp"
#include "MacroUtilities.hpp"
#include "ModelManager.hpp"

const PropertyCollection& FontTTF::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.AddCommand("filename", MemberCommandPtr<FontTTF>(&FontTTF::_CMD_LoadFont));
	properties.Add<int>("rendersize", offsetof(FontTTF, _size));
	DO_ONCE_END;

	return properties;
}

void FontTTF::_CMD_LoadFont(const Buffer<String>& args)
{
	if (args.GetSize() > 0)
	{
		String fp = args[0];

		const Buffer<String> paths = Engine::Instance().pFontManager->GetPaths();
		for (size_t i = 0; i < paths.GetSize(); ++i)
		{
			fp = paths[i] + args[0];

			if (IO::FileExists(fp.GetData()))
				break;
		}

		FT_Error error = FT_New_Face(Engine::Instance().GetFTLibrary(), fp.GetData(), 0, &_face);
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

float FontTTF::CalculateStringWidth(const char* string, float scaleX) const
{
	float width = 0.f;

	float scale = scaleX / (float)_size;

	for (const char* c = string; *c != '\0'; ++c)
	{
		if (*c == '\t')
		{
			const TTFGlyph* space = _charMap.Get(' ');
			float stopWidth = (space->advance >> 6) * scale * 5.f;
			float nextStop = Maths::Trunc(width, stopWidth) + stopWidth;

			width = nextStop;
		}
		else
		{
			const TTFGlyph* glyph = _charMap.Get(*c);
			if (glyph)
				width += (glyph->advance >> 6) * scale;
		}
	}

	return width;
}

void FontTTF::RenderString(const char* string, const Transform& transform, float lineHeight) const
{
	Transform t;
	Vector3 advanceDirection = t.GetRightVector();
	Vector3 upDirection = t.GetUpVector();
	Vector3 downDirection = -1.f * upDirection;

	t.SetPosition(transform.GetPosition());

	float scale = (transform.GetScale()[0] / (float)_size);

	float line = 0.f;
	float currentLineW = 0.f;

	float yOffset = (float)_descender;

	t.Move(downDirection * yOffset);

	glActiveTexture(GL_TEXTURE0);

	GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVOffset, Vector2(0.f, 0.f));
	GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1.f, 1.f));

	GLProgram::Current().SetBool(DefaultUniformVars::boolIsFont, true);

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
				glBindTexture(GL_TEXTURE_2D, glyph->texID);

				t.SetScale(Vector3(glyph->size[0] * scale, glyph->size[1] * scale, 1.f));

				Vector3 v = t.GetPosition() + (advanceDirection * (glyph->bearing[0] + glyph->size[0] / 2.f) * scale) + (downDirection * (glyph->size[1] / 2.f - glyph->bearing[1]) * scale);

				GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model,
					Matrix::Transformation(v, t.GetRotation().GetQuat(), t.GetScale()));

				Engine::Instance().pModelManager->Plane()->Render();

				t.Move(advanceDirection * (float)(glyph->advance >> 6) * scale);
				currentLineW += (float)(glyph->advance >> 6) * scale;
			}
		}
	}

	GLProgram::Current().SetBool(DefaultUniformVars::boolIsFont, false);
}
