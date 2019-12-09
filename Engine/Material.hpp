#pragma once
#include "Asset.hpp"
#include "Engine.hpp"
#include "RenderChannel.hpp"
#include "String.hpp"

struct RenderParam;

class Material : public Asset
{
protected:
	RenderChannels _renderChannels;

	Material(RenderChannels channels) : _renderChannels(channels) { }

public:
	virtual ~Material() {}

	static Material* FromText(const String&);

	virtual void Apply(const RenderParam* param = nullptr) const = 0;

	RenderChannels GetRenderChannels() const { return _renderChannels; }
};
