#pragma once
#include <ELSys/AssetManager.hpp>
#include "Tile.hpp"

class TileManager : public AssetManager<Tile>
{
	virtual Tile* _CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context&) override;

public:
	TileManager() : AssetManager<Tile>() {}
	virtual ~TileManager() {}
};
