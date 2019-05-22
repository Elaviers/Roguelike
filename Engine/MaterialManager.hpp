#pragma once
#include "ResourceManager.hpp"
#include "FunctionPointer.hpp"
#include "Material.hpp"

class MaterialManager : public ResourceManager<Material*>
{
private:
	virtual bool _CreateResource(Material *&, const String&, const String&) override;

public:
	MaterialManager() {}
	virtual ~MaterialManager() {}
};
 