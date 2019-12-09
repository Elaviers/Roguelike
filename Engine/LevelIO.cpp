#include "LevelIO.hpp"
#include "Engine.hpp"
#include "Debug.hpp"
#include "IO.hpp"
#include "Map.hpp"
#include "Registry.hpp"

#include "EntBrush2D.hpp"
#include "EntBrush3D.hpp"
#include "EntRenderable.hpp"

constexpr const char *levelPrefix = "POO";
constexpr const byte currentVersion = LevelVersions::VERSION_2;

namespace LevelMessages
{
	enum Enum
	{
		STRING = 0,
	};
}


inline void WriteStringMessage(BufferWriter<byte> &buffer, const String &string, uint16 id)
{
	buffer.Write_byte(0);
	buffer.Write_byte(LevelMessages::STRING);
	buffer.Write_uint16(id);
	buffer.Write_string(string.GetData());
}

bool LevelIO::Write(const Entity &world, const char *filename)
{
	Buffer<byte> buffer1, buffer2;
	BufferWriter<byte> writer1(buffer1), writer2(buffer2);
	NumberedSet<String> strings;

	writer1.Write_string(levelPrefix);
	writer1.Write_byte(currentVersion);

	world.WriteAllToFile(writer2, strings);

	auto stringBuffer = strings.ToKVBuffer();
	for (uint32 i = 0; i < stringBuffer.GetSize(); ++i)
		WriteStringMessage(writer1, stringBuffer[i]->second, stringBuffer[i]->first);

	Buffer<byte> finalBuffer = buffer1 + buffer2;
	return IO::WriteFile(filename, finalBuffer.Data(), (uint32)finalBuffer.GetSize());
}

bool LevelIO::Read(Entity &world, const char *filename)
{
	Buffer<byte> buffer = IO::ReadFile(filename);

	if (buffer.GetSize() > 0)
	{
		NumberedSet<String> strings;
		BufferReader<byte> reader(buffer);
		
		if (reader.Read_string() != levelPrefix)
		{
			Debug::Error("Is this even a level file? I don't think so.");
			return false;
		}

		if (reader.Read_byte() != currentVersion)
		{
			Debug::Error("Unsupported level version");
			return false;
		}

		while (reader.Valid())
		{
			EntityID id = EntityID(reader.Read_byte());

			if (id == EntityID::NONE)
			{
				switch (reader.Read_byte())
				{
				case LevelMessages::STRING:
				{
					uint16 stringID = reader.Read_uint16();
					strings[stringID] = reader.Read_string();
				}
					break;
				}
			}
			else
			{
				Entity *obj = Engine::Instance().registry.GetNode(id)->New();
				if (obj)
				{
					obj->SetParent(&world);
					obj->ReadData(reader, strings);
				}
				else Debug::Error("Unsupported object ID");
			}
		}

		return true;
	}

	return false;
}
