#include "LevelGeneration.hpp"
#include "LevelSegmentPicker.hpp"
#include <Engine/Entity.hpp>
#include <Engine/EntityIterator.hpp>
#include <Engine/LevelIO.hpp>
#include <Engine/Random.hpp>
#include <Engine/String.hpp>

inline RandomBag<const EntConnector*> GetAllAvailableConnectors(const Entity& root, Random &random)
{
	RandomBag<const EntConnector*> result;

	for (ConstEntityIterator it(&root); it.IsValid(); ++it)
	{
		const EntConnector* asConnector = dynamic_cast<const EntConnector*>(&*it);
		if (asConnector && !asConnector->connected)
			result.Add(asConnector, 1.f);
	}

	return result;
}

Entity* CreateConnectedSegment(Entity& world, const EntConnector &parentConnector, LevelSegmentPicker& picker, Random &random, unsigned int depth)
{
	Transform pcWt = parentConnector.GetWorldTransform();

	RandomBag<const LevelSegmentPicker::BagItem> availableSegments;
	picker.GetAvailableOptions(random, depth, availableSegments);

	while (availableSegments.GetRemainingWeight() > 0.f)
	{
		const LevelSegmentPicker::BagItem& item = availableSegments.TakeNext(random, 0.f);
		const Entity* segment = item.segment;

		RandomBag<const EntConnector*> connectors = GetAllAvailableConnectors(*segment, random);
		while (connectors.GetRemainingWeight() > 0.f)
		{
			const EntConnector* connector = connectors.TakeNext(random);

			Transform connectorWt = connector->GetWorldTransform();
			if (connectorWt.GetScale() == pcWt.GetScale())
			{
				float angle = (pcWt.GetRotationEuler()[1] - connectorWt.GetRotationEuler()[1]) - 180.f;
				Transform segWt = segment->GetWorldTransform();

				Vector3 connectorPosAfterSegmentRotation = VectorMaths::RotateAbout(connectorWt.GetPosition(), segWt.GetPosition(), Vector3(0.f, angle, 0.f));

				segWt.AddRotation(Vector3(0.f, angle, 0.f));
				segWt.Move(pcWt.GetPosition() - connectorPosAfterSegmentRotation);

				bool cannotFit = false;
				for (ConstEntityIterator it(segment); it.IsValid(); ++it)
				{
					Transform t = it->GetRelativeTransform();

					//Use slightly smaller version
					t.SetScale(t.GetScale() * .95f);

					for (const Entity* e = it->GetParent(); e && e != segment; e = e->GetParent())
						t = t * e->GetRelativeTransform();

					t = t * segWt;

					if (it->GetCollider() && world.AnyPartOverlapsCollider(*it->GetCollider(), t) == EOverlapResult::OVERLAPPING)
					{
						cannotFit = true;
						break;
					}
				}
					
				if (cannotFit) continue;

				//Okay, we can add to world now
				item.TakeFromRelevantBag(1.f);

				Entity* newSegment = Entity::Create();
				newSegment->SetParent(&world);
				newSegment->SetWorldTransform(segWt);
				newSegment->CloneChildrenFrom(*segment);

				for (EntityIterator it(newSegment); it.IsValid(); ++it)
				{
					EntConnector* newConnector = dynamic_cast<EntConnector*>(&*it);
					if (newConnector && newConnector->GetWorldPosition().AlmostEqual(pcWt.GetPosition(), .1f))
						newConnector->connected = true;
				}

				return newSegment;
			}
		}
	}

	return nullptr;
}

void GenerateConnectedSegments(Entity &world, const Entity &src, LevelSegmentPicker &picker, Random &random, unsigned int depth)
{
	if (depth <= 0) return;

	RandomBag<const EntConnector*> connectors = GetAllAvailableConnectors(src, random);
	
	while (connectors.GetRemainingWeight() > 0.f)
	{
		//Ignore the next line please!!
		EntConnector* srcConnector = const_cast<EntConnector*>(connectors.TakeNext(random));
		srcConnector->connected = true;
		Entity* newSegment = CreateConnectedSegment(world, *srcConnector, picker, random, depth);
		if (newSegment)
			GenerateConnectedSegments(world, *newSegment, picker, random, depth - 1);
	}
}

bool LevelGeneration::GenerateLevel(Entity& root, const String &string)
{
	Random random;

	LevelSegmentPicker picker = LevelSegmentPicker::FromString(string, ROOT_DIR);

	int depth = 10;

	Buffer<String> lines = string.Split("\r\n");
	for (uint32 i = 0; i < lines.GetSize(); ++i)
	{
		Buffer<String> tokens = lines[i].Split(" ");

		if (tokens.GetSize() >= 2)
		{
			if (tokens[0] == "depth")
				depth = tokens[1].ToInt();
		}
	}

	const Entity* initialSegment = picker.TakeNextSegment(random, depth);
	Entity *segmentClone = Entity::Create();
	segmentClone->SetParent(&root);
	segmentClone->CloneChildrenFrom(*initialSegment);
	
	GenerateConnectedSegments(root, *segmentClone, picker, random, depth - 1);

	return true;
}
