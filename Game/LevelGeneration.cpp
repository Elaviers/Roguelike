#include "LevelGeneration.hpp"
#include "LevelBag.hpp"
#include <Engine/Entity.hpp>
#include <Engine/LevelIO.hpp>
#include <Engine/Random.hpp>
#include <Engine/String.hpp>

EntConnector* GetRandomConnector(const Buffer<Entity*> &objects, Random &random)
{
	Buffer<uint32> indices;

	for (uint32 i = 0; i < objects.GetSize(); ++i)
		if (objects[i]->IsType<EntConnector>())
			indices.Add(i);

	if (indices.GetSize() == 0)
		return nullptr;

	return dynamic_cast<EntConnector*>(objects[indices[random.Next((uint32)indices.GetSize())]]);
}

Entity* CreateConnectedSegment(const EntConnector &connector, const LevelBag &bag, Random &random, int depth)
{
	const Entity &level = bag.GetNextLevel(random);
	const EntConnector *otherConnector = GetRandomConnector(level.Children(), random);

	if (!otherConnector)
		return nullptr;

	int angle = Direction2DFuncs::GetAngleOf(otherConnector->direction) - Direction2DFuncs::GetAngleOf(connector.direction);

	Vector3 levelMin, levelMax;

	for (uint32 i = 0; i < level.Children().GetSize(); ++i)
	{
		Bounds bounds = level.Children()[i]->GetWorldBounds();

		if (bounds.min[0] < levelMin[0] || bounds.min[1] < levelMin[1] || bounds.min[2] < levelMin[2])
			levelMin = bounds.min; 
		if (bounds.max[0] > levelMax[0] || bounds.max[1] > levelMax[1] || bounds.max[2] > levelMax[2])
			levelMax = bounds.max;
	}

	Vector3 centre = levelMax - levelMin;

	Entity *newSegment = Entity::Create();
	newSegment->SetRelativePosition(centre);
	newSegment->CloneChildrenFrom(level);
	return newSegment;
}

void GenerateConnectedSegments(Entity &world, const Entity &src, const LevelBag &bag, Random &random, int depth)
{
	for (uint32 i = 0; i < src.Children().GetSize(); ++i)
	{
		if (src.Children()[i]->IsType<EntConnector>())
		{
			const EntConnector *connector = reinterpret_cast<const EntConnector*>(src.Children()[i]);
			if (!connector->connected)
			{
				Entity *newSeg = CreateConnectedSegment(*connector, bag, random, depth - 1);
				if (newSeg)
					newSeg->SetParent(&world);
			}
		}
	}
}

bool LevelGeneration::GenerateLevel(Entity& root, const String &string)
{
	Random random;

	int depth = 10;
	LevelBag bag;

	Buffer<String> lines = string.Split("\r\n");
	for (uint32 i = 0; i < lines.GetSize(); ++i)
	{
		Buffer<String> tokens = lines[i].Split("=");

		if (tokens.GetSize() > 0)
		{
			if (tokens[0] == "depth")
				depth = tokens[0].ToInt();
			else
			{
				tokens = lines[i].Split(" ");

				if (tokens.GetSize() > 1)
					if (tokens[0] == "segment")
					{
						Entity *level = Entity::Create();
						if (LevelIO::Read(*level, CSTR(ROOT_DIR, tokens[1])))
							bag.AddLevel(*level, 1);
						else
							level->Delete();
					}
			}
		}
	}

	const Entity &lvl = bag.GetNextLevel(random);
	Entity *seg1 = Entity::Create();
	seg1->CloneChildrenFrom(lvl);
	
	GenerateConnectedSegments(root, *seg1, bag, random, depth);

	return true;
}
