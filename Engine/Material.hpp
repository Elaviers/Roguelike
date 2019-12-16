#pragma once
#include "Asset.hpp"
#include "Engine.hpp"
#include "RenderChannels.hpp"
#include "String.hpp"

struct RenderParam;

class Material : public Asset
{
protected:
	RenderChannels _RenderChannelss;

	Material(RenderChannels channels) : _RenderChannelss(channels) { }

public:
	virtual ~Material() {}

	static Material* FromText(const String&);

	virtual void Apply(const RenderParam* param = nullptr) const = 0;

	RenderChannels GetRenderChannelss() const { return _RenderChannelss; }
};
