#include "LevelGeneration.h"
#include "LevelBag.h"
#include <Engine/GameObject.h>

void GenerateSegment(const LevelBag &bag, const Connector &connector, Collection &collection, int depth)
{
	const Level &level = bag.GetLevel();
	const Connector &otherConnector = level.Connectors()[Maths::Random() * level.Connectors().GetSize()];

	int angle = Direction2DFuncs::GetAngleOf(otherConnector.direction) - Direction2DFuncs::GetAngleOf(connector.direction);

	GameObject *newSegment = new GameObject();

	Vector3 levelMin, levelMax;

	for (uint32 i = 0; i < level.ObjectCollection().Objects().GetSize(); ++i)
	{
		const GameObject* obj = level.ObjectCollection().Objects()[i];

		if (obj)
		{
			Bounds bounds = obj->GetWorldBounds();

			if (bounds.min[0] < levelMin[0] || bounds.min[1] < levelMin[1] || bounds.min[2] < levelMin[2])
				levelMin = bounds.min;
			if (bounds.max[0] > levelMax[0] || bounds.max[1] > levelMax[1] || bounds.max[2] > levelMax[2])
				levelMax = bounds.max;
		}
	}

	Vector3 centre = levelMax - levelMin;
	newSegment->transform.SetPosition(centre);

	for (uint32 i = 0; i < level.ObjectCollection().Objects().GetSize(); ++i)
	{
		if (level.ObjectCollection().Objects()[i])
		{
			GameObject *newObject = nullptr;
			Utilities::CopyBytes(level.ObjectCollection().Objects()[i], newObject, newObject->SizeOf());

			newObject->SetParent(newSegment);
		}
	}

}

Collection LevelGeneration::GenerateLevel(const String &string)
{
	int depth = 10;
	LevelBag bag;

	/////
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
				{
					if (tokens[0] == "segment")
					{
						bag.AddLevel(CSTR(root + tokens[1]), 1);
					}
				}
			}
		}
	}

	return Collection();
}
