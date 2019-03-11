#pragma once
#include "ResourceManager.h"
#include "FunctionPointer.h"
#include "Material.h"

class MaterialManager : public ResourceManager<Material*>
{
private:
	virtual bool _CreateResource(Material *&, const String&, const String&) override;

public:
	MaterialManager() {}
	virtual ~MaterialManager() {}
};
 