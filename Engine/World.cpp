#include "World.hpp"
#include "Console.hpp"
#include "WorldObject.hpp"
#include <ELCore/Sorting.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELMaths/Frustum.hpp>
#include <ELMaths/LineSegment.hpp>

World::AdditionalObjectData::AdditionalObjectData(WorldObject* object) : 
	spd(object, 0, [this](SharedPointerData<WorldObject>&) { TrySelfDelete(); })
{

}

void World::AdditionalObjectData::ObjectDestroyed()
{
	fixedBodies.Clear();
	physicsBodies.Clear();
	spd.SetPtr(nullptr);
	TrySelfDelete();
}

void World::AdditionalObjectData::TrySelfDelete()
{
	if (spd.GetReferenceCount() == 0 && spd.GetPtr() == nullptr)
		delete this;
}

World::~World()
{
	for (const Pair<const uint32, AdditionalObjectData*>& kv : _additionalData)
		if (kv.second)
			kv.second->ObjectDestroyed();

	for (WorldObject* object : _objects)
		delete object;
}

World::AdditionalObjectData& World::_GetAdditionalObjectData(WorldObject* object)
{
	if (AdditionalObjectData** ptr = object->GetWorld()._additionalData.TryGet(object->GetUID()))
		return **ptr;

	return *(object->GetWorld()._additionalData[object->GetUID()] = new AdditionalObjectData(object));
}

WorldObject* World::CloneObject(const WorldObject& object, bool deep)
{
	World* objWorld = &object.GetWorld();
	World*& currentObjectWorld = const_cast<World*&>(object._world);
	
	//todo: this is pretty hacky... the source object's world temporarily changes to this world so the copy constructor works properly
	currentObjectWorld = this;
	WorldObject* clone = object._Clone();
	currentObjectWorld = objWorld;
	
	clone->_OnCloned();
	_objects.EmplaceBack(clone);

	if (deep)
		for (const WorldObject* child : object.GetChildren())
			CloneObject(*child, true)->SetParent(clone, false);

	return clone;
}

WorldObject* World::FindObject(const Text& name) const
{
	for (WorldObject* object : _objects)
		if (object->GetName() == name)
			return object;

	return nullptr;
}

WorldObject* World::FindObject(uint32 uid) const
{
	for (WorldObject* object : _objects)
		if (object->GetUID() == uid)
			return object;

	return nullptr;
}

void World::Initialise()
{
	Console* pConsole = _context->GetPtr<Console>();
	pConsole->Cvars().CreateVar("Ents", CommandPtr(&World::_CMD_List, *this));
	pConsole->Cvars().CreateVar("Ent", CommandPtr(&World::_CMD_Ent, *this));
}

void World::Clear()
{
	for (const Pair<const uint32, AdditionalObjectData*>& kv : _additionalData)
		if (kv.second)
			kv.second->ObjectDestroyed();

	for (WorldObject* object : _objects)
		delete object;

	_additionalData.Clear();
	_objects.Clear();
}

void World::Update(float deltaTime)
{
	for (List<WorldObject*>::Iterator it = _objects.begin(); it;)
		if ((*it)->IsPendingDestruction())
		{
			if (AdditionalObjectData** adata = _additionalData.TryGet((*it)->GetUID()))
			{
				(*adata)->ObjectDestroyed();
				*adata = nullptr;

				//todo-- remove adata from map!
			}

			delete *it;
			it = _objects.Remove(it);
		}
		else ++it;

	_physics.Simulate(deltaTime);

	for (WorldObject* object : _objects)
		object->Update(deltaTime);
}

void World::Render(RenderQueue& q, const Frustum* f) const
{
	if (f)
	{
		for (const WorldObject* object : _objects)
			if (object->IsVisible(*f))
				object->Render(q);
	}
	else
	{
		for (const WorldObject* object : _objects)
			object->Render(q);
	}
}

void World::RenderID(RenderQueue& q, const Frustum* f)
{
	for (WorldObject* object : _objects)
		if (f == nullptr || object->IsVisible(*f))	
			object->RenderID(q, object->GetUID());
}

WorldObject* World::DecodeIDMapValue(uint32 red, uint32 green) const
{
	return red ? FindObject(red) : nullptr;
}

Handle<FixedBody> World::CreateFixedBody(WorldObject* owner)
{
	FixedBody& body = _physics.NewFixedBody();
	body.SetOwner(owner);
	_GetAdditionalObjectData(owner).fixedBodies.Add(&body);
	return Handle<FixedBody>(body, [this](const FixedBody& body) { _physics.RemoveBody(body); });
}

Handle<FixedBody> World::CreateFixedBody(WorldObject* owner, const FixedBody& other)
{
	FixedBody& body = _physics.NewFixedBody(other);
	body.SetOwner(owner);
	_GetAdditionalObjectData(owner).fixedBodies.Add(&body);
	return Handle<FixedBody>(body, [this](const FixedBody& body) { _physics.RemoveBody(body); });
}

Handle<PhysicsBody> World::CreatePhysicsBody(WorldObject* owner)
{
	PhysicsBody& body = _physics.NewPhysicsBody();
	body.SetOwner(owner);
	_GetAdditionalObjectData(owner).physicsBodies.Add(&body);
	return Handle<PhysicsBody>(body, [this](const PhysicsBody& body) { _physics.RemoveBody(body); });
}

Handle<PhysicsBody> World::CreatePhysicsBody(WorldObject* owner, const PhysicsBody& other)
{
	PhysicsBody& body = _physics.NewPhysicsBody(other);
	body.SetOwner(owner);
	_GetAdditionalObjectData(owner).physicsBodies.Add(&body);
	return Handle<PhysicsBody>(body, [this](const PhysicsBody& body) { _physics.RemoveBody(body); });
}

void _AddUniqueOverlaps(Buffer<WorldObject*>& results, const Collider& collider, const Transform& transform, const PhysSimulation& phys, bool includeTouches)
{
	PhysSimulation::FindOverlapsResult overlaps;
	size_t oCount = phys.FindOverlaps(overlaps, collider, transform, false);

	if (oCount)
	{
		size_t i = results.GetSize();
		results.Grow(oCount);

		for (const PhysSimulation::OverlapInformation<FixedBody>& info : overlaps.fixedBodies)
		{
			WorldObject* obj = (WorldObject*)info.body->GetUserData();
			if ((includeTouches || info.type == EOverlapResult::OVERLAPPING) && IteratorUtils::IndexOf(results.begin(), results.end(), obj) != IteratorUtils::INVALID_INDEX)
				results[i++] = obj;
		}

		for (const PhysSimulation::OverlapInformation<PhysicsBody>& info : overlaps.physicsBodies)
		{
			WorldObject* obj = (WorldObject*)info.body->GetUserData();
			if ((includeTouches || info.type == EOverlapResult::OVERLAPPING) && IteratorUtils::IndexOf(results.begin(), results.end(), obj) != IteratorUtils::INVALID_INDEX)
				results[i++] = obj;
		}
	}
}

Buffer<WorldObject*> World::FindOverlaps(const Collider& collider, const Transform& transform, bool includeTouching) const
{
	Buffer<WorldObject*> results;
	_AddUniqueOverlaps(results, collider, transform, _physics, includeTouching);
	return results;
}

Buffer<WorldObject*> World::FindObjectOverlaps(const WorldObject* object, bool includeTouching) const
{
	Buffer<WorldObject*> results;

	AdditionalObjectData*const* data = object->GetWorld()._additionalData.TryGet(object->GetUID());
	if (data)
	{
		for (const FixedBody* fb : (*data)->fixedBodies)
			_AddUniqueOverlaps(results, fb->Collision(), fb->GetTransform(), _physics, includeTouching);

		for (const PhysicsBody* pb : (*data)->physicsBodies)
			_AddUniqueOverlaps(results, pb->Collision(), pb->GetTransform(), _physics, includeTouching);
	}

	while (true)
	{
		WorldObject** it = IteratorUtils::FirstWhere(results.begin(), results.end(), [object](const auto& obj) { return (*obj) == nullptr || **obj == *object; });
		if (it == results.end()) break;
		results.Remove(results.PositionToIndex(it));
	}

	return results;
}

Buffer<RaycastResult> World::Raycast(const Ray& ray, ECollisionChannels channels) const
{
	Buffer<RaycastHitInformation> pResults;
	Buffer<RaycastResult> results;

	if (_physics.Raycast(pResults, ray, channels))
	{
		int i = 0;
		results.SetSize(pResults.GetSize());

		for (RaycastHitInformation& pResult : pResults)
		{
			RaycastResult& result = results[i++];
			result.fBody = pResult.fBody;
			result.pBody = pResult.pBody;
			result.time = pResult.time;

			result.object = (WorldObject*)(result.fBody ? result.fBody->GetUserData() : result.pBody->GetUserData());
		}
	}
	
	Sorting::Quicksort<RaycastResult, float>(results.begin(), 0, results.GetSize() - 1, [](const RaycastResult& rr) { return rr.time; });
	
	return results;
}

//this function is yucky
EOverlapResult ProcessOverlaps(const Collider& collider, const Transform& transform, 
	const Buffer<FixedBody*>& fbs, const Buffer<PhysicsBody*>& pbs, Vector3* penetrationOut)
{
	EOverlapResult result = EOverlapResult::SEPERATE;

	for (const FixedBody* fb : fbs)
	{
		if (collider.BroadOverlapCheck(transform, fb->Collision(), fb->GetTransform()))
		{
			Vector3 penetration;

			EOverlapResult r = collider.NarrowOverlapCheck(transform, fb->Collision(), fb->GetTransform(), nullptr, penetrationOut ? &penetration : nullptr);
			if (r != EOverlapResult::SEPERATE)
			{
				result = Maths::Max(r, result);

				if (penetrationOut)
					*penetrationOut = Maths::Max(*penetrationOut, penetration);
			}
		}
	}

	for (const PhysicsBody* pb : pbs)
	{
		if (collider.BroadOverlapCheck(transform, pb->Collision(), pb->GetTransform()))
		{
			Vector3 penetration;

			EOverlapResult r = collider.NarrowOverlapCheck(transform, pb->Collision(), pb->GetTransform(), nullptr, penetrationOut ? &penetration : nullptr);
			if (r != EOverlapResult::SEPERATE)
			{
				result = Maths::Max(r, result);

				if (penetrationOut)
					*penetrationOut = Maths::Max(*penetrationOut, penetration);
			}
		}
	}

	return result;
}

EOverlapResult World::ObjectsOverlap(const WorldObject& a, const WorldObject& b, Vector3* penetration) const
{
	AdditionalObjectData*const* adata = a.GetWorld()._additionalData.TryGet(a.GetUID());
	AdditionalObjectData*const* bdata = b.GetWorld()._additionalData.TryGet(b.GetUID());

	EOverlapResult result = EOverlapResult::SEPERATE;

	if (adata && bdata)
	{
		bool findPenetration = penetration != nullptr;
		Vector3 p;

		for (const FixedBody* fb : (*adata)->fixedBodies)
		{
			EOverlapResult r = ProcessOverlaps(fb->Collision(), fb->GetTransform(), (*bdata)->fixedBodies, (*bdata)->physicsBodies, findPenetration ? &p : nullptr);
			result = Maths::Max(r, result);
			if (findPenetration) *penetration = Maths::Max(*penetration, p);
		}

		for (const PhysicsBody* pb : (*adata)->physicsBodies)
		{
			EOverlapResult r = ProcessOverlaps(pb->Collision(), pb->GetTransform(), (*bdata)->fixedBodies, (*bdata)->physicsBodies, findPenetration ? &p : nullptr);
			result = Maths::Max(r, result);
			if (findPenetration) *penetration = Maths::Max(*penetration, p);
		}
	}

	return result;
}

//BEUH
void ProcessClosestPoints(const Collider& collider, const Transform& transform,
	const Buffer<FixedBody*>& fbs, const Buffer<PhysicsBody*>& pbs, Vector3& pointA, Vector3& pointB, float& d2)
{
	Vector3 pta, ptb;

	for (const FixedBody* fb : fbs)
	{
		if (collider.MinimumDistanceTo(transform, fb->Collision(), fb->GetTransform(), pta, ptb, nullptr) > 0.f)
			if ((pta - ptb).LengthSquared() < d2)
			{
				pointA = pta;
				pointB = ptb;
				d2 = (pointA - pointB).LengthSquared();
			}
	}

	for (const PhysicsBody* pb : pbs)
	{
		if (collider.MinimumDistanceTo(transform, pb->Collision(), pb->GetTransform(), pta, ptb, nullptr) > 0.f)
			if ((pta - ptb).LengthSquared() < d2)
			{
				pointA = pta;
				pointB = ptb;
				d2 = (pointA - pointB).LengthSquared();
			}
	}
}

float World::CalculateClosestPoints(const WorldObject& a, const WorldObject& b, Vector3& pointA, Vector3& pointB) const
{
	AdditionalObjectData* const* adata = a.GetWorld()._additionalData.TryGet(a.GetUID());
	AdditionalObjectData* const* bdata = b.GetWorld()._additionalData.TryGet(b.GetUID());

	float d2 = INFINITY;

	if (adata && bdata)
	{
		for (const FixedBody* fb : (*adata)->fixedBodies)
			ProcessClosestPoints(fb->Collision(), fb->GetTransform(), (*bdata)->fixedBodies, (*bdata)->physicsBodies, pointA, pointB, d2);

		for (const PhysicsBody* pb : (*adata)->physicsBodies)
			ProcessClosestPoints(pb->Collision(), pb->GetTransform(), (*bdata)->fixedBodies, (*bdata)->physicsBodies, pointA, pointB, d2);
	}

	return d2;
}

//IO
#include "ObjectIOContext.hpp"
#include "ObjectType.hpp"
#include <ELSys/IO.hpp>

namespace WorldVersions
{
	enum : byte //This should be a byte... hopefully I won't need more than 256 level versions.
	{
		VERSION_1 = 0,
		VERSION_2 = 1,		//Connectors are Entities instead of metadata
		VERSION_3 = 2,		//Geometry is stored at top level instead of inside OGeometryCollection
		VERSION_4 = 3		//Geometry data now handled by OGeometryCollection objects
	};
}

constexpr const char* worldPrefix = "POO";
constexpr const byte currentVersion = WorldVersions::VERSION_4;

namespace LevelMessages
{
	enum EPropertyFlags : byte
	{
		STRING = 0,
		ENTITIES = 1,
		GEOMETRY = 2 //VERSION 3 ONLY
	};
}


bool World::ReadObjects(ByteReader& reader, WorldObject* parent)
{
	if (reader.Read<String>() != worldPrefix)
	{
		Debug::Error("Is this even a level file? I don't think so.");
		return false;
	}

	byte version = reader.Read_byte();
	if (version != WorldVersions::VERSION_2 && version != WorldVersions::VERSION_3 && version != WorldVersions::VERSION_4)
	{
		Debug::Error("Unsupported level version");
		return false;
	}

	//Here we go
	ObjectIOContext ioContext = { .context = *_context };
	ObjectRegister* objectTypes = _context->GetPtr<ObjectRegister>();

	while (reader.IsValid())
	{
		byte id = reader.Read_byte();

		if (id == (byte)EObjectID::NONE)
		{
			switch (reader.Read_byte())
			{
			case LevelMessages::STRING:
			{
				uint16 id = reader.Read_uint16();
				ioContext.strings.Set(id, reader.Read<String>());
			}
				break;
			case LevelMessages::ENTITIES:
				//Do nothing
				break;
			case LevelMessages::GEOMETRY:
				Debug::Error("Top-level geometry data is deprecated for newer levels! No additional level data can be loaded!");
				return true;

			default:
				Debug::Error(CSTR("Unhandled level message!"));
			}
		}
		else
		{
			if (version == WorldVersions::VERSION_2)
			{
				if (id == (byte)EObjectID::LEVEL_CONNECTOR)
				{
					reader.IncrementIndex(9 * 4); //TRANSFORM (9 floats)
					reader.Read<String>();
					reader.IncrementIndex(1 + 6 * 4); // 1 byte + 6 floats
					Debug::Message("This is a version 2 level; a level connector has been removed", "Hey");
					continue;
				}
			}

			const ObjectType* type = objectTypes->GetType((EObjectID)id);
			if (type)
			{
				WorldObject* obj = type->New(*this);
				if (obj)
				{
					obj->Read(reader, ioContext);

					if (parent && obj->GetParent() == nullptr)
						obj->SetParent(parent, false);
				}
				else Debug::Error(CSTR("Could not create an object of type [", type->GetName(), "]... WTF?"));
			}
			else Debug::Error(CSTR("Unsupported object ID : ", id));
		}
	}

	return true;
}

bool World::ReadObjects(const char* filename, WorldObject* parent)
{
	Buffer<byte> buffer = IO::ReadFile(filename);

	if (buffer.GetSize() > 0)
	{
		ByteReader reader(buffer);
		return ReadObjects(reader, parent);
	}

	return false;
}

inline void WriteStringMessage(ByteWriter& buffer, const String& string, uint16 id)
{
	buffer.Write_byte(0);
	buffer.Write_byte(LevelMessages::STRING);
	buffer.Write_uint16(id);
	string.Write(buffer);
}

void WriteObjectRecursive(WorldObject* object, ByteWriter& writer, ObjectIOContext& ctx)
{
	writer.Write_byte((byte)object->GetTypeID());
	object->Write(writer, ctx);

	for (WorldObject* child : object->GetChildren())
		WriteObjectRecursive(child, writer, ctx);
}

bool World::WriteObjects(ByteWriter& writer, WorldObject* parent) const
{
	ObjectIOContext ioContext = { .context = *_context, .relParent = parent };
	Buffer<byte> buffer1, buffer2;
	ByteWriter writer1(buffer1), writer2(buffer2);

	writer1.Write(worldPrefix);
	writer1.Write_byte(currentVersion);

	writer2.Write_byte(0);
	writer2.Write_byte(LevelMessages::ENTITIES);

	if (parent)
	{
		for (WorldObject* object : parent->GetChildren())
			WriteObjectRecursive(object, writer2, ioContext);
	}
	else
		for (WorldObject* object : _objects)
		{
			writer.Write_byte((byte)object->GetTypeID());
			object->Write(writer, ioContext);
		}

	for (const Pair<const uint32, String>& kv : ioContext.strings)
		WriteStringMessage(writer1, kv.second, kv.first);

	writer.Write(buffer1.begin(), buffer1.GetSize());
	writer.Write(buffer2.begin(), buffer2.GetSize());
	return true;
}

bool World::WriteObjects(const char* filename, WorldObject* parent) const
{
	Buffer<byte> data;
	ByteWriter writer(data);

	if (WriteObjects(writer, parent))
		return IO::WriteFile(filename, data.begin(), (uint32)data.GetSize());

	return false;
}

void World::_CMD_List(const Array<String>& tokens, const Context& ctx)
{
	Console* console = ctx.GetPtr<Console>();

	for (WorldObject* object : _objects)
		console->Print(CSTR("[", object->GetClassString(), "]", " \"", object->GetName(), "\"\t\t0x", String::FromInt(object->GetUID(), 0, 16), '\n'));
}

void World::_CMD_Ent(const Array<String>& tokens, const Context& ctx)
{
	if (tokens.GetSize() >= 1)
	{
		WorldObject* obj = FindObject(Text::FromString(tokens[0]));

		if (obj)
		{
			Console* console = ctx.GetPtr<Console>();

			if (tokens.GetSize() >= 2)
			{
				Buffer<String> objTokens(&tokens[1], tokens.GetSize() - 1);
				console->Print(obj->GetProperties().HandleCommand(obj, objTokens, ctx).begin());
			}
			else
			{
				const Buffer<Property*> properties = obj->GetProperties().GetAll();

				for (size_t i = 0; i < properties.GetSize(); ++i)
					console->Print(CSTR(properties[i]->GetName(), "\t\t", properties[i]->GetTypeName(), "\t\t<", properties[i]->GetAsString(obj, ctx), ">\n"));
			}
		}
	}
}
