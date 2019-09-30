#pragma once
#include "Asset.hpp"
#include "Engine.hpp"
#include "RenderChannel.hpp"
#include "String.hpp"
#include "TextureManager.hpp"

struct RenderParam;

class Material : public Asset
{
protected:
	byte _renderChannels;

	byte _mips;
	uint16 _mag;

	Material(byte channels) : _renderChannels(channels), _mips(0), _mag(GL_NEAREST) { }

	String _GetMag() { return "{MATERIAL._MAG}"; }
	void _SetMag(const String& name);

public:
	virtual ~Material() {}

	virtual const PropertyCollection& GetProperties() override;

	static Material* FromText(const String&);

	virtual void Apply(const RenderParam *param = nullptr) const {}

	inline byte GetRenderChannels() const { return _renderChannels; }
};
