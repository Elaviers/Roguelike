#include "Material_Surface.hpp"
#include "RenderCommand.hpp"
#include "RenderEntry.hpp"
#include "TextureManager.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/MacroUtilities.hpp>
#include <ELCore/Utilities.hpp>
#include <ELSys/Debug.hpp>

const PropertyCollection& MaterialSurface::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE(properties.AddCommand("tex", MemberCommandPtr<MaterialSurface>(&MaterialSurface::CMD_tex)));

	return properties;
}

MaterialSurface::MaterialSurface(const SharedPointer<const Texture>& diffuse, 
	const SharedPointer<const Texture>& normal, 
	const SharedPointer<const Texture>& specular, 
	const SharedPointer<const Texture>& reflection) : Material(ERenderChannels::SURFACE), _diffuse(diffuse), _normal(normal), _specular(specular), _reflection(reflection)
{
}

void MaterialSurface::CMD_tex(const Buffer<String>& args, const Context& ctx)
{
	if (args.GetSize() >= 2)
	{
		TextureManager* textureManager = ctx.GetPtr<TextureManager>();
		if (textureManager)
		{
			if (args[0] == "diffuse")
				_diffuse = textureManager->Get(args[1], ctx);
			else if (args[0] == "normal")
				_normal = textureManager->Get(args[1], ctx);
			else if (args[0] == "specular")
				_specular = textureManager->Get(args[1], ctx);
			else if (args[0] == "reflection")
				_reflection = textureManager->Get(args[1], ctx);
		}
	}
}

void MaterialSurface::Apply(RenderEntry& e, const MaterialParam *param) const
{
	if (_diffuse) e.AddSetTexture(*_diffuse, 0);
	else e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);

	if (_normal) e.AddSetTexture(*_normal, 1);
	else e.AddSetTexture(RCMDSetTexture::Type::UNIT_NORMAL, 1);
 
	if (_specular) e.AddSetTexture(*_specular, 2);
	else e.AddSetTexture(RCMDSetTexture::Type::GREY, 2);

	if (_reflection) e.AddSetTexture(*_reflection, 3);
	else e.AddSetTexture(RCMDSetTexture::Type::BLACK, 3);

}
