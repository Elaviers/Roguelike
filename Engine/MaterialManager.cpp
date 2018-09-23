#include "MaterialManager.h"
#include "IO.h"
#include "TextureManager.h"
#include "Utilities.h"

constexpr const char *extension = ".txt";

const Material* MaterialManager::GetMaterial(const String &nameIn)
{
	String name = nameIn.ToLower();

	Material *mat = _map.Find(name);

	if (mat) return mat;
	else if (nameIn.GetLength() > 0)
	{
		String fs = IO::ReadFileString((_rootPath + name + extension).ToLower().GetData());

		if (fs.GetLength() != 0)
		{
			Material &newMat = _map[name];

			Utilities::LowerString(fs);
			Buffer<String> lines = fs.Split("\r\n");

			for (unsigned int i = 0; i < lines.GetSize(); ++i)
			{
				Buffer<String> tokens = lines[i].Split("=");

				if (tokens[0] == "diffuse")
					newMat.SetDiffuse(_textureManager->GetTexture(tokens[1]));
				else if (tokens[0] == "normal")
					newMat.SetNormal(_textureManager->GetTexture(tokens[1]));
				else if (tokens[0] == "specular")
					newMat.SetSpecular(_textureManager->GetTexture(tokens[1]));
				else if (tokens[0] == "reflection")
					newMat.SetReflection(_textureManager->GetTexture(tokens[1]));
			}

			return &newMat;
		}
	}

	return nullptr;
}
