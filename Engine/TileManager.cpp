#include "TileManager.hpp"

Tile* TileManager::_CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context& ctx)
{
	return new Tile(Tile::FromText(data, ctx));
}
