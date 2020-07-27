#include "MaterialManager.hpp"
#include "IO.hpp"
#include "Material_Grid.hpp"
#include "Material_Surface.hpp"
#include "Material_Sprite.hpp"
#include "TextureManager.hpp"
#include <ELCore/Utilities.hpp>
#include <ELSys/Debug.hpp>

Material* MaterialManager::_CreateResource(const String& data, const String& name, const String& ext, const Context& ctx)
{
	Material* mat = Material::FromText(data, ctx);

	if (mat == nullptr)
		Debug::Error(CSTR("Could not load material \"", name, '\"'));

	return mat;
}

void MaterialManager::Initialise(const TextureManager& tm)
{
	_MapValue("white").SetPtr(new MaterialSurface(tm.White(), tm.UnitNormal(), tm.White()));
}
