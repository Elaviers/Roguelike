#include "Font_TTF.h"
#include "Engine.h"
#include "GLProgram.h"
#include "FontManager.h"
#include "ModelManager.h"

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
			Debug::Error(CSTR("Could not load font file \"" + fp + '\"'));
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
				Debug::PrintLine(CSTR("COULD NOT LOAD CHAR \'" + (char)i) + '\'');
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
				Vector2(_face->glyph->bitmap.width, _face->glyph->bitmap.rows),
				Vector2(_face->glyph->bitmap_left, _face->glyph->bitmap_top),
				_face->glyph->advance.x
			};

			_charMap[(char)i] = glyph;
		}

		FT_Done_Face(_face);
	}
}

float FontTTF::CalculateStringWidth(const char* string, float scaleX) const
{
	float width = 0.f;

	float scale = scaleX / (float)_size;

	for (const char* c = string; *c != '\0'; ++c)
	{
		const TTFGlyph* glyph = _charMap.Find(*c);
		if (glyph)
			width += (glyph->advance >> 6) * scale;
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

	float yOffset = (_face->descender >> 6);

	t.Move(downDirection * yOffset);

	GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVOffset, Vector2(0.f, 0.f));
	GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1.f, 1.f));

	GLProgram::Current().SetBool(DefaultUniformVars::boolIsFont, true);

	for (const char* c = string; *c != '\0'; ++c)
	{
		if (*c == '\n' && lineHeight)
		{
			++line;

			t = transform;
			t.Move(downDirection * (yOffset + lineHeight * line));
		}
		else
		{
			const TTFGlyph* glyph = _charMap.Find(*c);
			if (glyph)
			{
				glBindTexture(GL_TEXTURE_2D, glyph->texID);

				t.SetScale(Vector3(glyph->size[0] * scale, glyph->size[1] * scale, 1.f));

				Vector3 v = t.GetPosition() + (advanceDirection * (glyph->bearing[0] + glyph->size[0] / 2.f) * scale) + (downDirection * (glyph->size[1] / 2.f - glyph->bearing[1]) * scale);

				GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model,
					Matrix::Transformation(v, t.GetRotation().GetQuat(), t.GetScale()));

				Engine::Instance().pModelManager->Plane().Render();

				t.Move(advanceDirection * (glyph->advance >> 6) * scale);
			}
		}
	}

	GLProgram::Current().SetBool(DefaultUniformVars::boolIsFont, false);
}