#pragma once
#include "Texture.hpp"
#include "Material.hpp"
#include <ELCore/SharedPointer.hpp>

class MaterialSprite : public Material
{
	SharedPointer<const Texture> _diffuse;

	void CMD_texmgr(const Buffer<String>& args, const Context& ctx);

public:
	MaterialSprite(const SharedPointer<const Texture>& diffuse = SharedPointer<const Texture>()) : Material(ERenderChannels::SPRITE), _diffuse(diffuse) {}

	virtual ~MaterialSprite() {}

	virtual const PropertyCollection& GetProperties() override;

	virtual void Apply(RenderEntry&, const MaterialParam* param = nullptr) const override { _diffuse->Bind(0); }
};
