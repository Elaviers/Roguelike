#pragma once
#include <ELCore/Buffer.hpp>

class World;
class WorldObject;

namespace EngineUtilities
{

	//returns true if selection was changed
	bool WorldIMGUI(World&, Buffer<WorldObject*>& selectedObjects);
}
