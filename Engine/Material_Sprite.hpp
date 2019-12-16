#pragma once
#include "Texture.hpp"
#include "Material.hpp"

class MaterialSprite : public Material
{
	SharedPointer<const Texture> _diffuse;

	void _CMD_tex(const Buffer<String>& args);

public:
	MaterialSprite(const SharedPointer<const Texture>& diffuse = SharedPointer<const Texture>()) : Material(RenderChannels::SPRITE), _diffuse(diffuse) {}

	virtual ~MaterialSprite() {}

	virtual const PropertyCollection& GetProperties() override;

	virtual void Apply(const RenderParam* param = nullptr) const override { _diffuse->Bind(0); }
};
