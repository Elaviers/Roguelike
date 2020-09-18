#include "World.hpp"
#include "EntityIterator.hpp"
#include <ELCore/Context.hpp>
#include <ELMaths/Frustum.hpp>
#include <ELMaths/LineSegment.hpp>

World::~World()
{
	for (Geometry* g : _geometry)
		delete g;
}

void World::Clear(const Context& ctx)
{
	for (Geometry* g : _geometry)
		delete g;

	_geometry.Clear();

	_entRoot.DeleteChildren(ctx);
}

void World::Update(float deltaTime)
{
	_entRoot.UpdateAll(deltaTime);
}

void World::Render(RenderQueue& q, const Frustum& f) const
{
	for (const Geometry* g : _geometry)
	{
		const Bounds& b = g->GetBounds();
		if (f.OverlapsAABB(b.min, b.max))
			g->Render(q);
	}

	_entRoot.RenderAll(q, f);
}

void World::GetOverlaps(List<Pair<EOverlapResult, Vector3>>& results, const Collider& collider, const Transform& transform, const Entity* ignore, const Vector3& sweep) const
{
	Vector3 p;
	EOverlapResult r;

	ConstEntityIterator it(&_entRoot);
	while (it.IsValid())
	{
		if (!ignore || (it->GetUID() != ignore->GetUID() && !it->IsChildOf(ignore)))
		{
			r = it->OverlapsCollider(collider, transform, sweep, &p);
			if (r != EOverlapResult::SEPERATE)
				results.Add(Pair(r, p));
		}

		it = it.Next();
	}

	for (const Geometry* g : _geometry)
	{
		r = g->Overlaps(collider, transform, sweep, &p);
		if (r != EOverlapResult::SEPERATE)
			results.Add(Pair(r, p));
	}
}


//IO
#include "Registry.hpp"
#include <ELSys/IO.hpp>

namespace LevelVersions
{
	enum : byte //This should be a byte... hopefully I won't need more than 256 level versions.
	{
		VERSION_1 = 0,
		VERSION_2 = 1,		//Connectors are Entities instead of metadata
		VERSION_3 = 2
	};
}

constexpr const char* levelPrefix = "POO";
constexpr const byte currentVersion = LevelVersions::VERSION_3;

namespace LevelMessages
{
	enum ELevelMessage : byte
	{
		STRING = 0,
		ENTITIES = 1,
		GEOMETRY = 2
	};
}

inline void WriteStringMessage(ByteWriter &buffer, const String &string, uint16 id)
{
	buffer.Write_byte(0);
	buffer.Write_byte(LevelMessages::STRING);
	buffer.Write_uint16(id);
	string.Write(buffer);
}

bool World::Read(const char* filename, const Context& ctx)
{
	Buffer<byte> buffer = IO::ReadFile(filename);

	if (buffer.GetSize() > 0)
	{
		NumberedSet<String> strings;
		ByteReader reader(buffer);

		if (reader.Read<String>() != levelPrefix)
		{
			Debug::Error("Is this even a level file? I don't think so.");
			return false;
		}

		byte version = reader.Read_byte();
		if (version != LevelVersions::VERSION_2 && version != LevelVersions::VERSION_3)
		{
			Debug::Error("Unsupported level version");
			return false;
		}

		//Here we go
		Registry<Entity>* entRegistry = ctx.GetPtr<Registry<Entity>>();
		Registry<Geometry>* geometryRegistry = ctx.GetPtr<Registry<Geometry>>();

		bool geometryMode = false;
		while (reader.IsValid())
		{
			byte id = reader.Read_byte();

			if (id == (byte)EEntityID::NONE)
			{
				switch (reader.Read_byte())
				{
				case LevelMessages::STRING:
					strings[reader.Read_uint16()].Read(reader);
					break;
				case LevelMessages::ENTITIES:
					geometryMode = false;
					break;
				case LevelMessages::GEOMETRY:
					geometryMode = true;
					break;
				}
			}
			else
			{
				if (!geometryMode)
				{
					if (version == LevelVersions::VERSION_2)
					{
						if (id == (byte)EEntityID::LEVEL_CONNECTOR)
						{
							reader.IncrementIndex(9 * 4); //TRANSFORM (9 floats)
							reader.Read<String>();
							reader.IncrementIndex(1 + 6 * 4); // 1 byte + 6 floats
							Debug::Message("This is a version 2 level, so a level connector has been removed", "Hey");
							continue;
						}
					}

					RegistryNodeBase<Entity>* node = entRegistry->GetNode(id);
					if (node)
					{
						Entity* obj = node->New();
						if (obj)
						{
							obj->SetParent(&_entRoot);
							obj->ReadData(reader, strings, ctx);
						}
						else Debug::Error("Could not create a new entity while loading level");
					}
					else
						Debug::Error(CSTR("Unsupported entity ID : ", id));
				}
				else
				{
					RegistryNodeBase<Geometry>* node = geometryRegistry->GetNode(id);
					if (node)
					{
						Geometry* g = node->New();
						if (g)
						{
							_geometry.Add(g);
							g->ReadData(reader, strings, ctx);
						}
						else Debug::Error("Could not create new geometry while loading level");
					}
					else Debug::Error(CSTR("Unsupported geometry ID :", id));
				}
			}
		}

		return true;
	}

	return false;

}

bool World::Write(const char* filename, const Context& ctx) const
{
	Buffer<byte> buffer1, buffer2;
	ByteWriter writer1(buffer1), writer2(buffer2);
	NumberedSet<String> strings;

	writer1.Write(levelPrefix);
	writer1.Write_byte(currentVersion);

	writer2.Write_byte(0);
	writer2.Write_byte(LevelMessages::ENTITIES);
	_entRoot.WriteAllToFile(writer2, strings, ctx);

	writer2.Write_byte(0);
	writer2.Write_byte(LevelMessages::GEOMETRY);
	for (Geometry* g : _geometry)
	{
		writer2.Write_byte(g->GetTypeID());
		g->WriteData(writer2, strings, ctx);
	}

	auto stringBuffer = strings.ToKVBuffer();
	for (uint32 i = 0; i < stringBuffer.GetSize(); ++i)
		WriteStringMessage(writer1, stringBuffer[i]->second, stringBuffer[i]->first);

	Buffer<byte> finalBuffer = buffer1 + buffer2;
	return IO::WriteFile(filename, finalBuffer.Data(), (uint32)finalBuffer.GetSize());
}
