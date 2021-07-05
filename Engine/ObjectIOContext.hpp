#pragma once
#include <ELCore/Context.hpp>
#include <ELCore/NumberedSet.hpp>
#include <ELCore/String.hpp>

struct ObjectIOContext
{
	const Context& context;
	Hashmap<uint32, class WorldObject*> objects;
	NumberedSet<String> strings;
	uint32 index;
	const WorldObject* relParent;
};
