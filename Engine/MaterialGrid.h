#pragma once
#include "Material.h"
#include "GLTexture.h"
#include "Buffer.h"
#include "UVRect.h"

class MaterialGrid : public Material
{
	const GLTexture *_texture;

	int _columns;

	Buffer<UVRect> _elements;

public:
	MaterialGrid() {}
	virtual ~MaterialGrid() {}

	virtual void FromString(const String &string) override;

	virtual void Apply(const RenderParam *param) const override;

	inline const UVRect& GetElement(int r, int c) const { return _elements[r * _columns + c]; }
};
