#pragma once
#include "BufferIterator.hpp"
#include "Types.hpp"
#include "Vector.hpp"
#include "GameObject.hpp"

namespace LevelVersions
{
	enum //This should be a byte... hopefully I won't need more than 256 level versions.
	{
		VERSION_1 = 0,
		VERSION_2 = 1		//Connectors are gameobjects instead of metadata

	};
}

namespace LevelIO
{
	bool Read(GameObject &out, const char *filename);
	bool Write(const GameObject &object, const char *filename);

}