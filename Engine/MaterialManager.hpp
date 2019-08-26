#pragma once
#include "AssetManager.hpp"
#include "FunctionPointer.hpp"
#include "Material.hpp"

class MaterialManager : public AssetManager<Material>
{
private:
	virtual Material* _CreateResource(const String&, const String&) override;

public:
	MaterialManager() : AssetManager("") {}
	virtual ~MaterialManager() {}

	void Initialise();
};
 