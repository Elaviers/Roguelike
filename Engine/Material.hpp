#pragma once
#include "Asset.hpp"
#include "CvarMap.hpp"
#include "Engine.hpp"
#include "ShaderChannel.hpp"
#include "String.hpp"
#include "TextureManager.hpp"

struct RenderParam;

class Material : public Asset
{
protected:
	CvarMap _cvars;

	byte _shaderChannels;

	Material(byte channels) : _shaderChannels(channels) 
	{ 
		_cvars.CreateVar("mag", CommandPtr(Engine::Instance().pTextureManager, &TextureManager::CMD_mag));
		_cvars.CreateVar("mips", CommandPtr(Engine::Instance().pTextureManager, &TextureManager::CMD_mips));
	}

	virtual void _ReadText(const String& string) override
	{
		Buffer<String> lines = string.ToLower().Split("\r\n");
		for (size_t i = 0; i < lines.GetSize(); ++i)
			_cvars.HandleCommand(lines[i]);
	}

public:
	virtual ~Material() {}

	static Material* FromText(const String&);

	virtual void Apply(const RenderParam *param = nullptr) const {}
	virtual void BindTextures() const {}

	inline byte GetShaderChannels() const { return _shaderChannels; }
};
