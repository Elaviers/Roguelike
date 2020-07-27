#pragma once
#include <ELCore/Types.hpp>
#include "Entity.hpp"

namespace LevelVersions
{
	enum //This should be a byte... hopefully I won't need more than 256 level versions.
	{
		VERSION_1 = 0,
		VERSION_2 = 1,		//Connectors are Entitys instead of metadata
		VERSION_3 = 2
	};
}

namespace LevelIO
{
	bool Read(Entity &out, const char *filename, const Context& ctx);
	bool Write(const Entity &object, const char *filename, const Context& ctx);

}
