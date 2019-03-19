#include "MaterialManager.h"
#include "Debug.h"
#include "IO.h"
#include "Material_Grid.h"
#include "Material_Surface.h"
#include "TextureManager.h"
#include "Utilities.h"

bool MaterialManager::_CreateResource(Material*& material, const String &name, const String &data)
{
	if (data.GetLength() != 0)
	{
		int splitIndex = data.IndexOfAny("\r\n");
		String firstLine = data.SubString(0, splitIndex).ToLower();
		Material* newMaterial;

		if (firstLine == "surface")
			newMaterial = new MaterialSurface();
		else if (firstLine == "grid")
			newMaterial = new MaterialGrid();
		else
		{
			Debug::Error(CSTR("Unknown material type for material \"" + name + '\"'));
			return false;
		}

		if (data[splitIndex + 1] != '\0')
			newMaterial->FromString(data.SubString(splitIndex + 1));

		material = newMaterial;
		return true;
	}

	return false;
}
