#pragma once
#include "Material.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "UVRect.hpp"

class MaterialGrid : public Material
{
	const Texture *_texture;

	Buffer<int> _rowHeights;
	Buffer<int> _columnWidths;

	Buffer<UVRect> _elements;

	void _TryCalculateElements();

	void _CMD_texture(const Buffer<String> &args);
	void _CMD_rows(const Buffer<String> &args);
	void _CMD_columns(const Buffer<String> &args);

public:
	MaterialGrid() : Material(RenderChannel::ALL), _texture(nullptr)	
	{
		_cvars.CreateVar("texture", CommandPtr(this, &MaterialGrid::_CMD_texture));
		_cvars.CreateVar("rows", CommandPtr(this, &MaterialGrid::_CMD_rows));
		_cvars.CreateVar("columns", CommandPtr(this, &MaterialGrid::_CMD_columns));
	}

	virtual ~MaterialGrid() {}

	virtual void Apply(const RenderParam *param) const override;

	inline const UVRect& GetElement(int r, int c) const { return _elements[r * _columnWidths.GetSize() + c]; }
};
