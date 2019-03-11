#pragma once
#include "CvarMap.h"
#include "Engine.h"
#include "ShaderChannel.h"
#include "String.h"
#include "TextureManager.h"

struct RenderParam;

class Material
{
protected:
	CvarMap _cvars;

	byte _shaderChannels;

	Material(byte channels) : _shaderChannels(channels) 
	{ 
		_cvars.CreateVar("mag", CommandPtr(Engine::Instance().pTextureManager, &TextureManager::CMD_mag));
		_cvars.CreateVar("mips", CommandPtr(Engine::Instance().pTextureManager, &TextureManager::CMD_mips));
	}

	virtual ~Material() {}

public:
	void FromString(const String& string);

	virtual void Apply(const RenderParam *param = nullptr) const {}
	virtual void BindTextures() const {}

	inline byte GetShaderChannels() const { return _shaderChannels; }
};
