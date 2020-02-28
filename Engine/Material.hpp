#pragma once
#include "Asset.hpp"
#include "Engine.hpp"
#include "ERenderChannels.hpp"
#include "String.hpp"

struct RenderParam;

class Material : public Asset
{
protected:
	ERenderChannels _RenderChannelss;

	Material(ERenderChannels channels) : _RenderChannelss(channels) { }

public:
	virtual ~Material() {}

	static Material* FromText(const String&);

	virtual void Apply(const RenderParam* param = nullptr) const = 0;

	ERenderChannels GetRenderChannelss() const { return _RenderChannelss; }
};
