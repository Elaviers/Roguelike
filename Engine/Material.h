#pragma once
#include "ShaderChannel.h"
#include "String.h"

struct RenderParam;

class Material
{
protected:
	byte _shaderChannels;

	Material(byte channels) : _shaderChannels(channels) {}
	virtual ~Material() {}

public:
	virtual void FromString(const String &string) {}

	virtual void Apply(const RenderParam *param = nullptr) const {}
	virtual void BindTextures() const {}

	inline byte GetShaderChannels() const { return _shaderChannels; }
};
