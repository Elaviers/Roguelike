#pragma once
#include "ResourceManager.h"
#include "Material.h"

class MaterialManager : public ResourceManager<Material*>
{
public:
	MaterialManager() {}
	virtual ~MaterialManager() {}

	const Material* GetMaterial(const String &name);
};
 