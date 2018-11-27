#include "MaterialSurface.h"
#include "Engine.h"
#include "Error.h"
#include "Utilities.h"

void MaterialSurface::FromString(const String &stringIn)
{
	String string = stringIn.ToLower();
	Buffer<String> lines = string.Split("\r\n");

	for (unsigned int i = 0; i < lines.GetSize(); ++i)
	{
		Buffer<String> tokens = lines[i].Split("=");

		if (Engine::textureManager)
		{
			if (tokens[0] == "diffuse")
				_diffuse = Engine::textureManager->GetTexture(tokens[1]);
			else if (tokens[0] == "normal")
				_normal = Engine::textureManager->GetTexture(tokens[1]);
			else if (tokens[0] == "specular")
				_specular = Engine::textureManager->GetTexture(tokens[1]);
			else if (tokens[0] == "reflection")
				_reflection = Engine::textureManager->GetTexture(tokens[1]);
		}
		else Error("Texture manager not set!");
	}
}

void MaterialSurface::Apply(const RenderParam *unused) const
{
	if (_diffuse)
		_diffuse->Bind(0);

	if (_normal)
		_normal->Bind(1);

	if (_specular)
		_specular->Bind(2);

	if (_reflection)
		_reflection->Bind(3);
}
