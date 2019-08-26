#pragma once
#include "Texture.hpp"
#include "Material.hpp"

class MaterialSprite : public Material
{
	const Texture* _diffuse;

	void _CMD_tex(const Buffer<String>& args);

public:
	MaterialSprite(const Texture *diffuse = nullptr) : Material(RenderChannel::SPRITE), _diffuse(diffuse)
	{
		_cvars.CreateVar("tex", CommandPtr(this, &MaterialSprite::_CMD_tex));
	}
	virtual ~MaterialSprite() {}

	virtual void Apply(const RenderParam* param = nullptr) const override { _diffuse->Bind(0); }
};

