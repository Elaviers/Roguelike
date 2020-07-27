#pragma once
#include <ELSys/AssetManager.hpp>
#include "Material.hpp"
#include "TextureManager.hpp"
#include <ELCore/FunctionPointer.hpp>

class MaterialManager : public AssetManager<Material>
{
private:
	virtual Material* _CreateResource(const String&, const String&, const String&, const Context&) override;

public:
	MaterialManager() : AssetManager() {}
	virtual ~MaterialManager() {}

	void Initialise(const TextureManager&);
};
 