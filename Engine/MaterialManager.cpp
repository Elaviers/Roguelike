#include "MaterialManager.h"
#include "Debug.h"
#include "IO.h"
#include "MaterialGrid.h"
#include "MaterialSurface.h"
#include "TextureManager.h"
#include "Utilities.h"

bool MaterialManager::_CreateResource(Material*& material, const String &name, const String &data)
{
	if (data.GetLength() != 0)
	{
		//todo: using min here is not an optimised way of finding the first char equal to one of two values
		int splitIndex = Utilities::Min(data.IndexOf('\n'), data.IndexOf('\r'));
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
