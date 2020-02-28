#include "Material_Grid.hpp"
#include "GLProgram.hpp"
#include "Engine.hpp"
#include "MacroUtilities.hpp"
#include "RenderParam.hpp"
#include "TextureManager.hpp"

const PropertyCollection& MaterialGrid::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.AddCommand("texture", MemberCommandPtr<MaterialGrid>(&MaterialGrid::_CMD_texture));
	properties.AddCommand("rows", MemberCommandPtr<MaterialGrid>(&MaterialGrid::_CMD_rows));
	properties.AddCommand("columns", MemberCommandPtr<MaterialGrid>(&MaterialGrid::_CMD_columns));
	DO_ONCE_END;

	return properties;
}

void MaterialGrid::_CMD_texture(const Buffer<String> &args)
{
	if (args.GetSize() > 0)
	{
		_texture = Engine::Instance().pTextureManager->Get(args[0]);
		_TryCalculateElements();
	}
}

void MaterialGrid::_CMD_rows(const Buffer<String> &args)
{
	_rowHeights.Clear();

	for (size_t i = 0; i < args.GetSize(); ++i)
		_rowHeights.Add(args[i].ToInt());

	_TryCalculateElements();
}

void MaterialGrid::_CMD_columns(const Buffer<String> &args)
{
	_columnWidths.Clear();

	for (size_t i = 0; i < args.GetSize(); ++i)
		_columnWidths.Add(args[i].ToInt());

	_TryCalculateElements();
}

void MaterialGrid::_TryCalculateElements()
{
	size_t rows = _rowHeights.GetSize();
	size_t columns = _columnWidths.GetSize();

	if (rows && columns && _texture->IsValid())
	{
		_elements.SetSize(rows * columns);

		float y = 0;
		for (size_t r = 0; r < rows; ++r)
		{
			float h = (float)_rowHeights[r] / (float)_texture->GetHeight();

			float x = 0;
			for (size_t c = 0; c < columns; ++c)
			{
				size_t index = r * columns + c;
				float w = (float)_columnWidths[c] / (float)_texture->GetWidth();

				_elements[index].pos = Vector2(x, y);
				_elements[index].size = Vector2(w, h);

				x += w;
			}

			y += h;
		}
	}
}

void MaterialGrid::Apply(const RenderParam *param) const
{
	int r = 0;
	int c = 0;

	if (param && param->type == RenderParam::EType::GRID_PARAM)
	{
		r = param->gridData.row;
		c = param->gridData.column;
	}

	const UVRect &uv = GetElement(r, c);

	if (_texture) _texture->Bind(0);
	else GLTexture::Unbind(0);

	GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVOffset, uv.pos);
	GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, uv.size);
}
