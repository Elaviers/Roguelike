#pragma once
#include "BufferIterator.hpp"
#include "Types.hpp"
#include "Vector.hpp"
#include "Entity.hpp"

namespace LevelVersions
{
	enum //This should be a byte... hopefully I won't need more than 256 level versions.
	{
		VERSION_1 = 0,
		VERSION_2 = 1		//Connectors are Entitys instead of metadata

	};
}

namespace LevelIO
{
	bool Read(Entity &out, const char *filename);
	bool Write(const Entity &object, const char *filename);

}
