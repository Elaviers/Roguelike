#include "Material_Grid.hpp"
#include "MaterialParam.hpp"
#include "RenderCommand.hpp"
#include "RenderEntry.hpp"
#include "TextureManager.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/MacroUtilities.hpp>

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

void MaterialGrid::_CMD_texture(const Buffer<String> &args, const Context& ctx)
{
	if (args.GetSize() > 0)
	{
		TextureManager* textureManager = ctx.GetPtr<TextureManager>();
		if (textureManager)
		{
			_texture = textureManager->Get(args[0], ctx);
			_TryCalculateElements();
		}
	}
}

void MaterialGrid::_CMD_rows(const Buffer<String> &args, const Context& ctx)
{
	_rowHeights.Clear();

	for (size_t i = 0; i < args.GetSize(); ++i)
		_rowHeights.Add(args[i].ToInt());

	_TryCalculateElements();
}

void MaterialGrid::_CMD_columns(const Buffer<String> &args, const Context& ctx)
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

void MaterialGrid::Apply(RenderEntry& e, const MaterialParam *param) const
{
	int r = 0;
	int c = 0;

	if (param && param->type == MaterialParam::EType::GRID_PARAM)
	{
		r = param->gridData.row;
		c = param->gridData.column;
	}

	const UVRect &uv = GetElement(r, c);

	if (_texture) e.AddSetTexture(*_texture, 0);
	else e.AddSetTexture(RCMDSetTexture::Type::BLACK, 0);

	e.AddSetUVOffset(uv.pos);
	e.AddSetUVScale(uv.size);
}
