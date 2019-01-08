#include "MaterialGrid.h"
#include "GLProgram.h"
#include "Engine.h"
#include "RenderParam.h"

void MaterialGrid::FromString(const String &string)
{
	Buffer<String> lines = string.Split("\r\n");

	Buffer<int> row_heights;
	Buffer<int> column_widths;

	for (uint32 i = 0; i < lines.GetSize(); ++i)
	{
		Buffer<String> tokens = lines[i].Split("=");

		if (tokens.GetSize() > 1)
		{
			if (tokens[0] == "texture")
				_texture = Engine::textureManager->GetTexture(tokens[1]);
			else if (tokens[0] == "rows")
			{
				Buffer<String> rows = tokens[1].Split(",");

				for (uint32 i = 0; i < rows.GetSize(); ++i)
					row_heights.Add(rows[i].ToInt());
			}
			else if (tokens[0] == "columns")
			{
				Buffer<String> columns = tokens[1].Split(",");

				for (uint32 i = 0; i < columns.GetSize(); ++i)
					column_widths.Add(columns[i].ToInt());
			}
		}
		else Engine::materialManager->HandleCommand(lines[i]);
	}

	uint32 rows = row_heights.GetSize();
	_columns = column_widths.GetSize();
	_elements.SetSize(rows * _columns);
	
	float y = 0;
	for (uint32 r = 0; r < rows; ++r)
	{
		float h = (float)row_heights[r] / (float)_texture->GetHeight();

		float x = 0;
		for (uint32 c = 0; c < _columns; ++c)
		{
			int index = r * _columns + c;
			float w = (float)column_widths[c] / (float)_texture->GetWidth();

			_elements[index].pos = Vector2(x, y);
			_elements[index].size = Vector2(w, h);

			x += w;
		}

		y += h;
	}
}

void MaterialGrid::Apply(const RenderParam *param) const
{
	int r = 0;
	int c = 0;

	if (param && param->type == RenderParam::Type::GRID_PARAM)
	{
		r = param->gridData.row;
		c = param->gridData.column;
	}

	const UVRect &uv = GetElement(r, c);

	BindTextures();

	GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVOffset, uv.pos);
	GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, uv.size);
}
