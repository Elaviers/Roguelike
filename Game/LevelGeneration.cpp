#include "LevelGeneration.hpp"
#include "RandomBag.hpp"
#include <Engine/Entity.hpp>
#include <Engine/EntityIterator.hpp>
#include <Engine/LevelIO.hpp>
#include <Engine/Random.hpp>
#include <Engine/String.hpp>

inline RandomBag<const EntConnector*> GetConnectors(const Entity& root, Random &random)
{
	RandomBag<const EntConnector*> result;

	for (ConstEntityIterator it(&root); it.IsValid(); ++it)
		if (it->IsType<EntConnector>())
			result.Add(dynamic_cast<const EntConnector*>(&*it), 1.f);

	return result;
}

Entity* CreateConnectedSegment(Entity& world, const EntConnector &parentConnector, RandomBag<const Entity*>& bag, Random &random)
{
	Transform pcWt = parentConnector.GetWorldTransform();

	while (bag.GetTotalWeight() > 0.f)
	{
		const Entity* level = bag.TakeNext(random);
		RandomBag<const EntConnector*> connectors = GetConnectors(*level, random);

		while (connectors.GetTotalWeight() > 0.f)
		{
			const EntConnector* connector = connectors.TakeNext(random);

			Transform connectorWt = connector->GetWorldTransform();
			if (connectorWt.GetScale() == pcWt.GetScale())
			{
				float angle = (pcWt.GetRotationEuler()[1] - connectorWt.GetRotationEuler()[1]) - 180.f;
				Transform levelWt = level->GetWorldTransform();

				Vector3 connectorPosAfterSegmentRotation = VectorMaths::RotateAbout(connectorWt.GetPosition(), levelWt.GetPosition(), Vector3(0.f, angle, 0.f));

				levelWt.AddRotation(Vector3(0.f, angle, 0.f));
				levelWt.Move(pcWt.GetPosition() - connectorPosAfterSegmentRotation);

				bool cannotFit = false;
				for (ConstEntityIterator it(level); it.IsValid(); ++it)
				{
					Transform t = it->GetRelativeTransform();

					//Use slightly smaller version
					t.SetScale(t.GetScale() * .95f);

					for (const Entity* e = it->GetParent(); e && e != level; e = e->GetParent())
						t = t * e->GetRelativeTransform();

					t = t * levelWt;

					if (it->GetCollider() && world.AnyPartOverlapsCollider(*it->GetCollider(), t) == EOverlapResult::OVERLAPPING)
					{
						cannotFit = true;
						break;
					}
				}
					
				if (cannotFit) continue;
				
				Entity* newSegment = Entity::Create();
				newSegment->SetParent(&world);
				newSegment->SetWorldTransform(levelWt);
				newSegment->CloneChildrenFrom(*level);
				return newSegment;
			}
		}
	}

	return nullptr;
}

void GenerateConnectedSegments(Entity &world, const Entity &src, RandomBag<const Entity*> &bag, Random &random, unsigned int depth)
{
	if (depth <= 0) return;

	for (uint32 i = 0; i < src.Children().GetSize(); ++i)
	{
		if (src.Children()[i]->IsType<EntConnector>())
		{
			EntConnector *connector = reinterpret_cast<EntConnector*>(src.Children()[i]);
			if (!connector->connected)
			{
				connector->connected = true;
				RandomBag<const Entity*> bagClone = bag;
				Entity* newSegment = CreateConnectedSegment(world, *connector, bagClone, random);
				if (newSegment)
					GenerateConnectedSegments(world, *newSegment, bag, random, depth - 1);
			}
		}
	}
}

bool LevelGeneration::GenerateLevel(Entity& root, const String &string)
{
	Random random;

	unsigned int depth = 10;
	RandomBag<const Entity*> bag;

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
							bag.Add(level, 1);
						else
							level->Delete();
					}
			}
		}
	}

	const Entity *lvl = bag.GetNext(random);
	Entity *seg1 = Entity::Create();
	seg1->SetParent(&root);
	seg1->CloneChildrenFrom(*lvl);
	
	GenerateConnectedSegments(root, *seg1, bag, random, depth);

	return true;
}
