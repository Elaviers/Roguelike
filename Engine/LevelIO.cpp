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
constexpr const byte currentVersion = LevelVersions::VERSION_3;

namespace LevelMessages
{
	enum ELevelMessage
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

		byte version = reader.Read_byte();
		if (version != LevelVersions::VERSION_2 && version != LevelVersions::VERSION_3)
		{
			Debug::Error("Unsupported level version");
			return false;
		}

		while (reader.Valid())
		{
			byte id = reader.Read_byte();

			if (id == (byte)EEntityID::NONE)
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
				if (version == LevelVersions::VERSION_2)
				{
					if (id == (byte)EEntityID::LEVEL_CONNECTOR)
					{
						reader.IncrementIndex(9*4); //TRANSFORM (9 floats)
						reader.Read_string();
						reader.IncrementIndex(1 + 6*4); // 1 byte + 6 floats
						Debug::Message("This is a version 2 level, so a level connector has been removed", "Hey");
						continue;
					}
				}

				RegistryNodeBase* node = Engine::Instance().registry.GetNode(id);
				if (node)
				{
					Entity* obj = Engine::Instance().registry.GetNode(id)->New();
					if (obj)
					{
						obj->SetParent(&world);
						obj->ReadData(reader, strings);
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
