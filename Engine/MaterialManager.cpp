#include "MaterialManager.hpp"
#include "Debug.hpp"
#include "IO.hpp"
#include "Material_Grid.hpp"
#include "Material_Surface.hpp"
#include "TextureManager.hpp"
#include "Utilities.hpp"

Material* MaterialManager::_CreateResource(const String &name, const String &data)
{
	Material* mat = nullptr;

	if (data.GetLength() > 0)
	{
		int splitIndex = data.IndexOfAny("\r\n");
		String firstLine = data.SubString(0, splitIndex).ToLower();
		
		if (firstLine == "surface")
			mat = new MaterialSurface();
		else if (firstLine == "grid")
			mat = new MaterialGrid();
		else
		{
			Debug::Error(CSTR("Unknown material type for material \"" + name + '\"'));
			return false;
		}

		if (data[splitIndex + 1] != '\0')
			mat->FromString(data.SubString(splitIndex + 1));
	}

	return mat;
}
