#include "LevelIO.h"
#include "Engine.h"
#include "Debug.h"
#include "IO.h"
#include "Map.h"
#include "Registry.h"

#include "ObjBrush2D.h"
#include "ObjBrush3D.h"
#include "ObjRenderable.h"

constexpr const char *levelPrefix = "POO";
constexpr const byte currentVersion = LevelVersions::VERSION_2;

namespace LevelMessages
{
	enum Enum
	{
		STRING = 0,
	};
}


inline void WriteStringMessage(BufferIterator<byte> &buffer, const String &string, uint16 id)
{
	buffer.Write_byte(0);
	buffer.Write_byte(LevelMessages::STRING);
	buffer.Write_uint16(id);
	buffer.Write_string(string.GetData());
}

bool LevelIO::Write(const GameObject &world, const char *filename)
{
	Buffer<byte> buffer1, buffer2;
	BufferIterator<byte> iterator1(buffer1), iterator2(buffer2);
	NumberedSet<String> strings;

	iterator1.Write_string(levelPrefix);
	iterator1.Write_byte(currentVersion);

	world.WriteAllToFile(iterator2, strings);

	auto stringBuffer = strings.ToBuffer();
	for (uint32 i = 0; i < stringBuffer.GetSize(); ++i)
		WriteStringMessage(iterator1, *stringBuffer[i].second, *stringBuffer[i].first);

	Buffer<byte> finalBuffer = buffer1 + buffer2;
	return IO::WriteFile(filename, finalBuffer.Data(), (uint32)finalBuffer.GetSize());
}

bool LevelIO::Read(GameObject &world, const char *filename)
{
	Buffer<byte> buffer = IO::ReadFile(filename);

	if (buffer.GetSize() > 0)
	{
		NumberedSet<String> strings;
		BufferIterator<byte> iterator(buffer);
		
		if (iterator.Read_string() != levelPrefix)
		{
			Debug::Error("Is this even a level file? I don't think so.");
			return false;
		}

		if (iterator.Read_byte() != currentVersion)
		{
			Debug::Error("Unsupported level version");
			return false;
		}

		while (iterator.Valid())
		{
			byte id = iterator.Read_byte();

			if (id == 0)
			{
				switch (iterator.Read_byte())
				{
				case LevelMessages::STRING:
				{
					uint16 stringID = iterator.Read_uint16();
					strings[stringID] = iterator.Read_string();
				}
					break;
				}
			}
			else
			{
				GameObject *obj = Engine::Instance().registry.GetNode(id)->New();
				if (obj)
				{
					obj->SetParent(&world);
					obj->ReadFromFile(iterator, strings);
				}
				else Debug::Error("Unsupported object ID");
			}
		}

		return true;
	}

	return false;
}
