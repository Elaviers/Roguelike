#include "Level.h"
#include "Engine.h"
#include "Error.h"
#include "IO.h"
#include "Map.h"

#include "Brush2D.h"
#include "Renderable.h"

void Connector::SaveToFile(BufferIterator<byte> &buffer) const
{
	buffer.Write_byte((byte)direction);
	buffer.Write_uint16(point1[0]);
	buffer.Write_uint16(point1[1]);
	buffer.Write_uint16(point1[2]);
	buffer.Write_uint16(point2[0]);
	buffer.Write_uint16(point2[1]);
	buffer.Write_uint16(point2[2]);
}

void Connector::LoadFromFile(BufferIterator<byte> &buffer)
{
	direction = (ConnectorDirection)buffer.Read_byte();
	point1[0] = buffer.Read_uint16();
	point1[1] = buffer.Read_uint16();
	point1[2] = buffer.Read_uint16();
	point2[0] = buffer.Read_uint16();
	point2[1] = buffer.Read_uint16();
	point2[2] = buffer.Read_uint16();
}

constexpr const char *levelPrefix = "POO";

namespace LevelMessages
{
	enum Enum
	{
		STRING = 0,
		CONNECTOR = 1
	};
}


inline void WriteStringMessage(BufferIterator<byte> &buffer, Map<String, uint16> &map, const String &string, uint16 id)
{
	if (map.Find(string) == nullptr)
	{
		map[string] = id;

		buffer.Write_byte(Engine::ObjectIDs::NOT_OBJECT);
		buffer.Write_byte(LevelMessages::STRING);
		buffer.Write_uint16(id);
		buffer.Write_string(string.GetData());
	}
}

void Level::WriteToFile(const char *filename) const
{
	Buffer<byte> buffer;
	BufferIterator<byte> iterator(buffer);

	Map<String, uint16> strings;
	uint16 nextStringID = 1;

	const Buffer<GameObject*> &objects = _collection.Objects();

	//Write
	iterator.Write_string(levelPrefix);
	iterator.Write_byte(LevelVersions::VERSION_1);

	for (uint32 i = 0; i < objects.GetSize(); ++i)
	{
		String string;

		if (dynamic_cast<Brush2D*>(objects[i]))
			WriteStringMessage(iterator, strings, ((Brush2D*)objects[i])->GetMaterialName(), nextStringID++);
		else if (dynamic_cast<Renderable*>(objects[i]))
		{
			WriteStringMessage(iterator, strings, ((Renderable*)objects[i])->GetMaterialName(), nextStringID++);
			WriteStringMessage(iterator, strings, ((Renderable*)objects[i])->GetModelName(), nextStringID++);
		}
	}

	for (uint32 i = 0; i < _connectors.GetSize(); ++i)
	{
		iterator.Write_byte(Engine::ObjectIDs::NOT_OBJECT);
		iterator.Write_byte(LevelMessages::CONNECTOR);
		_connectors[i].SaveToFile(iterator);
	}

	for (uint32 i = 0; i < objects.GetSize(); ++i)
		if (objects[i])
			objects[i]->SaveToFile(iterator, strings);

	IO::WriteFile(filename, buffer.Data(), buffer.GetSize());
}

template <typename T>
inline void CreateObject(Collection &world, BufferIterator<byte> &iterator, const Map<uint16, String> &strings)
{
	T *obj = world.NewObject<T>();
	obj->LoadFromFile(iterator, strings);
}

void Level::ReadFromFile(const char *filename)
{
	Buffer<byte> buffer = IO::ReadFile(filename);

	if (buffer.GetSize() > 0)
	{
		Map<uint16, String> strings;
		BufferIterator<byte> iterator(buffer);
		
		if (iterator.Read_string() != levelPrefix)
		{
			Error("Is this even a level file? I don't think so.");
			return;
		}

		if (iterator.Read_byte() != LevelVersions::VERSION_1)
		{
			Error("Unsupported level version");
			return;
		}

		while (iterator.Valid())
		{
			switch (iterator.Read_byte()) //Object class
			{
			case Engine::ObjectIDs::NOT_OBJECT:
				switch (iterator.Read_byte())
				{
					case LevelMessages::STRING:
						strings[iterator.Read_uint16()] = iterator.Read_string();
						break;

					case LevelMessages::CONNECTOR:
					{
						_connectors.Append(1);
						_connectors.Last().LoadFromFile(iterator);
					}
					break;
				}
				break;

			case Engine::ObjectIDs::RENDERABLE:
				CreateObject<Renderable>(_collection, iterator, strings);
				break;

			case Engine::ObjectIDs::BRUSH2D:
				CreateObject<Brush2D>(_collection, iterator, strings);
				break;

			default:
				Error("There's something very wrong with this level file. Has someone been playing with notepad?");
			}
		}

	}
	else Error("Why not try opening an actual level file, clever clogs?");
}
