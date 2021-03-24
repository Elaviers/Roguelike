#include "LevelGeneration.hpp"
#include "LevelSegmentPicker.hpp"
#include <ELCore/String.hpp>
#include <Engine/WorldObject.hpp>
#include <Engine/World.hpp>
#include <ELMaths/Random.hpp>
#include <ELSys/Time.hpp>

inline RandomBag<const OConnector*> GetAllAvailableConnectors(const WorldObject& parent, Random &random)
{
	RandomBag<const OConnector*> result;

	for (const WorldObject* object : parent.GetChildren())
	{
		const OConnector* asConnector = dynamic_cast<const OConnector*>(object);
		if (asConnector && !asConnector->connected)
			result.Add(asConnector, 1.f);
	}

	return result;
}

WorldObject* CreateConnectedSegment(World& world, const OConnector& parentConnector, LevelSegmentPicker& picker, Random &random, unsigned int depth)
{
	const Transform& pcWt = parentConnector.GetAbsoluteTransform();

	RandomBag<const LevelSegmentPicker::BagItem> availableSegments;
	picker.GetAvailableOptions(random, depth, availableSegments);

	while (availableSegments.GetRemainingWeight() > 0.f)
	{
		const LevelSegmentPicker::BagItem& item = availableSegments.TakeNext(random, 0.f);
		const WorldObject* segment = item.segment;

		RandomBag<const OConnector*> connectors = GetAllAvailableConnectors(*segment, random);
		while (connectors.GetRemainingWeight() > 0.f)
		{
			const OConnector* connector = connectors.TakeNext(random);

			Transform connectorWt = connector->GetAbsoluteTransform();
			if (connectorWt.GetScale() == pcWt.GetScale())
			{
				float angle = (pcWt.GetRotation().GetEuler().y - connectorWt.GetRotation().GetEuler().y) - 180.f;
				Transform segWt = segment->GetAbsoluteTransform();

				Vector3 connectorPosAfterSegmentRotation = VectorMaths::RotateAbout(connectorWt.GetPosition(), segWt.GetPosition(), Vector3(0.f, angle, 0.f));

				segWt.AddRotation(Vector3(0.f, angle, 0.f));
				segWt.Move(pcWt.GetPosition() - connectorPosAfterSegmentRotation);

				bool cannotFit = false;
				for (const WorldObject* object : world.GetObjects())
				{
					Transform t = object->GetRelativeTransform();

					//Use slightly smaller version
					t.SetScale(t.GetScale() * .95f);

					for (const WorldObject* obj = object->GetParent(); obj && obj != segment; obj = obj->GetParent())
						t = t * obj->GetRelativeTransform();

					t = t * segWt;

					if (world.FindObjectOverlaps(object, false).GetSize() > 0)
					{
						cannotFit = true;
						break;
					}
				}
					
				if (cannotFit) continue;

				//Okay, we can add to world now
				item.TakeFromRelevantBag(1.f);

				WorldObject* newSegment = world.CreateObject<WorldObject>();
				newSegment->SetParent(nullptr, true);
				newSegment->SetAbsoluteTransform(segWt);

				for (const WorldObject* child : segment->GetChildren())
					world.CloneObject(*child, true)->SetParent(newSegment, true);

				for (WorldObject* child : newSegment->GetChildren())
				{
					OConnector* newConnector = dynamic_cast<OConnector*>(child);
					if (newConnector && newConnector->GetAbsolutePosition().AlmostEquals(pcWt.GetPosition(), .1f))
						newConnector->connected = true;
				}

				return newSegment;
			}
		}
	}

	return nullptr;
}

void GenerateConnectedSegments(World &world, const WorldObject &src, LevelSegmentPicker &picker, Random &random, unsigned int depth)
{
	if (depth <= 0) return;

	RandomBag<const OConnector*> connectors = GetAllAvailableConnectors(src, random);
	
	while (connectors.GetRemainingWeight() > 0.f)
	{
		//Ignore the next line please!!
		OConnector* srcConnector = const_cast<OConnector*>(connectors.TakeNext(random));
		srcConnector->connected = true;
		WorldObject* newSegment = CreateConnectedSegment(world, *srcConnector, picker, random, depth);
		if (newSegment)
			GenerateConnectedSegments(world, *newSegment, picker, random, depth - 1);
	}
}

bool LevelGeneration::GenerateLevel(World& world, const String &string)
{
	const Context& ctx = *world.GetContext();
	Random random(Time::GetRandSeed());

	LevelSegmentPicker picker = LevelSegmentPicker::FromString(string, ROOT_DIR, world);

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

	const WorldObject* initialSegment = picker.TakeNextSegment(random, depth);

	WorldObject* segmentClone = world.CloneObject(*initialSegment, true);
	segmentClone->SetParent(nullptr, false);
	GenerateConnectedSegments(world, *segmentClone, picker, random, depth - 1);

	return true;
}
