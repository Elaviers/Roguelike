#pragma once
#include <ELSys/AssetManager.hpp>
#include "Tile.hpp"

class TileManager : public AssetManager<Tile>
{
	virtual Tile* _CreateResource(const Array<byte>& data, const String& name, const String& extension, const Context&) override;
	virtual bool _CreateAlternative(Tile*& resource, const String& name, const Context& ctx) override;

	virtual Buffer<const AssetManagerBase*> _GetFallbackManagers(const Context&) const override;

public:
	TileManager() : AssetManager<Tile>() {}
	virtual ~TileManager() {}
};
