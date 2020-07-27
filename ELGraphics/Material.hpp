#pragma once
#include <ELCore/Asset.hpp>
#include "ERenderChannels.hpp"
#include "RenderEntry.hpp"
#include <ELCore/String.hpp>

struct MaterialParam;

class Material : public Asset
{
protected:
	ERenderChannels _RenderChannels;

	Material(ERenderChannels channels) : _RenderChannels(channels) { }

public:
	virtual ~Material() {}

	static Material* FromText(const String&, const Context&);

	virtual void Apply(RenderEntry&, const MaterialParam* param = nullptr) const = 0;

	ERenderChannels GetRenderChannels() const { return _RenderChannels; }
};
