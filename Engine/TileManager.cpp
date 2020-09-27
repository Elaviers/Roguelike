#include "TileManager.hpp"

Tile* TileManager::_CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context& ctx)
{
	return new Tile(Tile::FromText(data, ctx));
}

#include <ELGraphics/MaterialManager.hpp>

bool TileManager::_CreateAlternative(Tile*& resource, const String& name, const Context& ctx)
{
	if (resource)
	{
		MaterialManager* materialManager = ctx.GetPtr<MaterialManager>();

		const SharedPointer<const Material> material = materialManager->Get(name, ctx);
		if (material)
		{
			resource->SetMaterial(material);
			return true;
		}
	}

	return false;
}

Buffer<const AssetManagerBase*> TileManager::_GetFallbackManagers(const Context& ctx) const
{
	return { (const AssetManagerBase*)ctx.GetPtr<MaterialManager>() };
}
