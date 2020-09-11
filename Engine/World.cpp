#include "World.hpp"
#include <ELCore/Context.hpp>

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
		g->Render(q); //todo: frustum cull

	_entRoot.RenderAll(q, f);
}


//IO
#include "Registry.hpp"
#include <ELSys/IO.hpp>

namespace LevelVersions
{
	enum //This should be a byte... hopefully I won't need more than 256 level versions.
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
	enum ELevelMessage
	{
		STRING = 0,
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

		while (reader.IsValid())
		{
			byte id = reader.Read_byte();

			if (id == (byte)EEntityID::NONE)
			{
				switch (reader.Read_byte())
				{
				case LevelMessages::STRING:
				{
					uint16 stringID = reader.Read_uint16();
					strings[stringID].Read(reader);
				}
				break;
				}
			}
			else
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

				Registry* registry = ctx.GetPtr<Registry>();
				RegistryNodeBase* node = registry->GetNode(id);
				if (node)
				{
					Entity* obj = registry->GetNode(id)->New();
					if (obj)
					{
						obj->SetParent(&_entRoot);
						obj->ReadData(reader, strings, ctx);
					}
					else Debug::Error("Could not create a new object");
				}
				else
				{
					Debug::Error(CSTR("Unsupported object ID : ", id));
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

	_entRoot.WriteAllToFile(writer2, strings, ctx);

	auto stringBuffer = strings.ToKVBuffer();
	for (uint32 i = 0; i < stringBuffer.GetSize(); ++i)
		WriteStringMessage(writer1, stringBuffer[i]->second, stringBuffer[i]->first);

	Buffer<byte> finalBuffer = buffer1 + buffer2;
	return IO::WriteFile(filename, finalBuffer.Data(), (uint32)finalBuffer.GetSize());
}
