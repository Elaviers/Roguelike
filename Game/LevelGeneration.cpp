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

Vector3 um(const Vector3& extent, const EDirection2D& dir)
{
	//return Vector3();

	switch (dir)
	{
	case EDirection2D::NORTH:
		return Vector3(0.f, 0.f, extent[2]);
		break;
	case EDirection2D::EAST:
		return Vector3(extent[0], 0.f, 0.f);
		break;
	case EDirection2D::SOUTH:
		return Vector3(0.f, 0.f, -extent[2]);
		break;
	}

	return Vector3(-extent[0], 0.f, 0.f);
}

Entity* CreateConnectedSegment(Entity& world, const EntConnector &parentConnector, RandomBag<const Entity*>& bag, Random &random)
{
	Transform pcWt = parentConnector.GetWorldTransform();
	Vector3 pcExtent = parentConnector.GetMax() - parentConnector.GetMin();
	Vector3 pcCentre = ((parentConnector.GetMin() + parentConnector.GetMax() + um(pcExtent, parentConnector.direction)) / 2.f) * pcWt.GetTransformationMatrix();

	while (bag.GetTotalWeight() > 0.f)
	{
		const Entity* level = bag.TakeNext(random);
		RandomBag<const EntConnector*> connectors = GetConnectors(*level, random);

		while (connectors.GetTotalWeight() > 0.f)
		{
			const EntConnector* connector = connectors.TakeNext(random);

			Vector3 extent = connector->GetMax() - connector->GetMin();
			if (extent == pcExtent || extent == Vector3(pcExtent[2], pcExtent[1], pcExtent[0]))
			{
				float angle = 180.f - Direction2DFuncs::GetAngleOf(connector->direction) - Direction2DFuncs::GetAngleOf(parentConnector.direction);
				Transform levelWt = level->GetWorldTransform();
				levelWt.AddRotation(Vector3(0.f, angle, 0.f));

				Vector3 centre = 
					((connector->GetMin() + connector->GetMax() + um(extent, connector->direction)) / 2.f) * 
					(
					connector->GetRelativeTransform().GetTransformationMatrix() * levelWt.GetTransformationMatrix());

				levelWt.Move(centre - pcCentre);

				bool cannotFit = false;
				for (ConstEntityIterator it(level); it.IsValid(); ++it)
				{
					Transform t = it->GetRelativeTransform();

					for (const Entity* e = it->GetParent(); e && e != level; e = e->GetParent())
						t = e->GetRelativeTransform() * t;

					t = levelWt * t;

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
			const EntConnector *connector = reinterpret_cast<const EntConnector*>(src.Children()[i]);
			if (!connector->connected)
			{
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
	seg1->CloneChildrenFrom(*lvl);
	
	GenerateConnectedSegments(root, *seg1, bag, random, 3);

	return true;
}
