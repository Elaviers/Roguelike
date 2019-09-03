#include "LevelGeneration.hpp"
#include "LevelBag.hpp"
#include <Engine/GameObject.hpp>
#include <Engine/LevelIO.hpp>
#include <Engine/Random.hpp>
#include <Engine/String.hpp>

ObjConnector* GetRandomConnector(const Buffer<GameObject*> &objects, Random &random)
{
	Buffer<uint32> indices;

	for (uint32 i = 0; i < objects.GetSize(); ++i)
		if (objects[i]->IsType<ObjConnector>())
			indices.Add(i);

	if (indices.GetSize() == 0)
		return nullptr;

	return dynamic_cast<ObjConnector*>(objects[indices[random.Next((uint32)indices.GetSize())]]);
}

GameObject* CreateConnectedSegment(const ObjConnector &connector, const LevelBag &bag, Random &random, int depth)
{
	const GameObject &level = bag.GetNextLevel(random);
	const ObjConnector *otherConnector = GetRandomConnector(level.Children(), random);

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

	GameObject *newSegment = GameObject::Create();
	newSegment->SetRelativePosition(centre);
	newSegment->CloneChildrenFrom(level);
	return newSegment;
}

void GenerateConnectedSegments(GameObject &world, const GameObject &src, const LevelBag &bag, Random &random, int depth)
{
	for (uint32 i = 0; i < src.Children().GetSize(); ++i)
	{
		if (src.Children()[i]->IsType<ObjConnector>())
		{
			const ObjConnector *connector = reinterpret_cast<const ObjConnector*>(src.Children()[i]);
			if (!connector->connected)
			{
				GameObject *newSeg = CreateConnectedSegment(*connector, bag, random, depth - 1);
				if (newSeg)
					newSeg->SetParent(&world);
			}
		}
	}
}

GameObject LevelGeneration::GenerateLevel(const String &string)
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
						GameObject *level = GameObject::Create();
						if (LevelIO::Read(*level, CSTR(root, tokens[1])))
							bag.AddLevel(*level, 1);
						else
							level->Delete();
					}
			}
		}
	}

	GameObject world;

	const GameObject &lvl = bag.GetNextLevel(random);
	GameObject *root = GameObject::Create();
	root->CloneChildrenFrom(lvl);
	
	GenerateConnectedSegments(world, *root, bag, random, depth);

	return world;
}
