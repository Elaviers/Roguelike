#include "Material_Surface.hpp"
#include "Engine.hpp"
#include "Debug.hpp"
#include "Utilities.hpp"
#include "TextureManager.hpp"

void MaterialSurface::_CMD_tex(const Buffer<String>& args)
{
	if (args.GetSize() >= 2)
	{
		if (args[0] == "diffuse")
			_diffuse = Engine::Instance().pTextureManager->Get(args[1]);
		else if (args[0] == "normal")
			_normal = Engine::Instance().pTextureManager->Get(args[1]);
		else if (args[0] == "specular")
			_specular = Engine::Instance().pTextureManager->Get(args[1]);
		else if (args[0] == "reflection")
			_reflection = Engine::Instance().pTextureManager->Get(args[1]);

	}
}

void MaterialSurface::BindTextures() const
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
