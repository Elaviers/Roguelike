#pragma once
#include "Map.h"
#include "Material.h"
#include "String.h"
#include "TextureManager.h"

class MaterialManager
{
private:
	Map<String, Material> _materials;

public:
	MaterialManager() {}
	~MaterialManager() {}

	inline const Material* Get(const char *name) const { return _materials.Find(name); }
	inline void Set(const char *name, const Material& material) { _materials.Set(name, material); }

	inline void MakeMaterial(TextureManager& textureManager, const char *name, const char *diffuse = "", const char *normal = "", const char *specular = "")
	{
		Material& mat = _materials[name];

		if (diffuse[0] != '\0')
			mat.SetDiffuse(textureManager.GetTexture(diffuse));

		if (normal[0] != '\0')
			mat.SetNormal(textureManager.GetTexture(normal));

		if (specular[0] != '\0')
			mat.SetSpecular(textureManager.GetTexture(specular));
	}
	
	String FindNameOfMaterial(const Material& material)
	{
		String name;

		auto found = _materials.FindFirstKey(material);
		if (found) name = *found;
		else name = "None";

		return name;
	}
};
 