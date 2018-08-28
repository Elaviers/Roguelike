#pragma once
#include "ResourceManagerBase.h"
#include "Material.h"

class TextureManager;

class MaterialManager : public ResourceManagerBase<Material>
{
public:
	MaterialManager() {}
	~MaterialManager() {}

	const Material* GetMaterial(TextureManager&, const String &name);
};
 