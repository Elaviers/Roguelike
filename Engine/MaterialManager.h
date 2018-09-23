#pragma once
#include "ResourceManagerBase.h"
#include "Material.h"

class TextureManager;

class MaterialManager : public ResourceManagerBase<Material>
{
	TextureManager *_textureManager;

public:
	MaterialManager(TextureManager &textureManager) : _textureManager(&textureManager) {}
	~MaterialManager() {}

	const Material* GetMaterial(const String &name);
};
 