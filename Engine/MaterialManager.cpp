#include "MaterialManager.h"
#include "Error.h"
#include "IO.h"
#include "MaterialGrid.h"
#include "MaterialSurface.h"
#include "TextureManager.h"
#include "Utilities.h"

constexpr const char *extension = ".txt";

const Material* MaterialManager::GetMaterial(const String &nameIn)
{
	String name = nameIn.ToLower();

	Material **mat = _map.Find(name);

	if (mat) return *mat;
	else if (nameIn.GetLength() > 0)
	{
		String fs = IO::ReadFileString((_rootPath + name + extension).ToLower().GetData());
		
		if (fs.GetLength() != 0)
		{
			//todo: using min here is not an optimised way of finding the first char equal to one of two values
			int splitIndex = Utilities::Min(fs.IndexOf('\n'), fs.IndexOf('\r'));
			String firstLine = fs.SubString(0, splitIndex).ToLower();
			Material *newMaterial;

			if (firstLine == "surface")
				newMaterial = new MaterialSurface();
			else if (firstLine == "grid")
				newMaterial = new MaterialGrid();
			else
			{
				Error(CSTR("Unknown material type for material \"" + name + '\"'));
				return nullptr;
			}

			if (fs[splitIndex + 1] != '\0')
				newMaterial->FromString(fs.SubString(splitIndex + 1));
			
			_map[name] = newMaterial;
			return newMaterial;
		}
	}

	return nullptr;
}
