#include "MaterialManager.hpp"
#include "Debug.hpp"
#include "IO.hpp"
#include "Material_Grid.hpp"
#include "Material_Surface.hpp"
#include "TextureManager.hpp"
#include "Utilities.hpp"

Material* MaterialManager::_CreateResource(const String &name, const String &data)
{
	Material* mat = Material::FromText(data);

	if (mat == nullptr)
		Debug::Error(CSTR("Could not load material \"" + name + '\"'));

	return mat;
}
