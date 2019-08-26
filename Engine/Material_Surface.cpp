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

#define BIND(PTEX, UNIT) if (PTEX) PTEX->Bind(UNIT); else GLTexture::Unbind(UNIT)

void MaterialSurface::Apply(const RenderParam *param) const
{
	BIND(_diffuse, 0);
	BIND(_normal, 1);
	BIND(_specular, 2);
	BIND(_reflection, 3);
}
