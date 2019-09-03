#include "MaterialManager.hpp"
#include "Debug.hpp"
#include "IO.hpp"
#include "Material_Grid.hpp"
#include "Material_Surface.hpp"
#include "Material_Sprite.hpp"
#include "TextureManager.hpp"
#include "Utilities.hpp"

Material* MaterialManager::_CreateResource(const String &name, const String &data)
{
	Material* mat = Material::FromText(data);

	if (mat == nullptr)
		Debug::Error(CSTR("Could not load material \"", name, '\"'));

	return mat;
}

void MaterialManager::Initialise()
{
	TextureManager* tm = Engine::Instance().pTextureManager;

	if (tm)
	{
		_MapValue("white") = new MaterialSurface(tm->White(), tm->NormalDefault(), tm->White());

		_MapValue("temp") = new MaterialSprite(tm->White());
	}
}
