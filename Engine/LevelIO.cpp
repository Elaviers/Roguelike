#include "LevelIO.h"
#include "IO.h"
#include "Map.h"

namespace LevelIO
{

	inline LevelObject& NewLevelObject(Buffer<LevelObject> &objBuffer, Buffer<byte> &fileBuffer, uint32 pos)
	{
		uint32 last = objBuffer.GetSize();
		objBuffer.Append(1);

		objBuffer[last].x = fileBuffer[++pos];
		objBuffer[last].y = fileBuffer[++pos];

		return objBuffer[last];
	}

	Buffer<LevelObject> ReadLevel(const char *filename)
	{
		Buffer<LevelObject> objects;
		Buffer<byte> fileData = IO::ReadFile(filename);

		if (fileData.GetSize())
		{
			byte currentMaterial, currentLayer, currentX;
			Map<byte, String> materials;
			

			for (uint32 i = 0; i < fileData.GetSize(); ++i)
			{
				switch (fileData[i])
				{
				case LevelMessages::MATERIALID:
					materials.Set(fileData[++i], String((char*)&fileData[++i]));
					while (fileData[i] != 0)
						i++;
					break;

				case LevelMessages::MATERIAL:
					currentMaterial = fileData[++i];
					break;
				case LevelMessages::LAYER:
					currentLayer = fileData[++i];
					break;
				case LevelMessages::X:
					currentX = fileData[++i];
					break;

				case LevelMessages::Y:
					if (fileData[++i] == 0)
					{

					}
					else
					{
						while (fileData[++i])
						{
							auto object = NewLevelObject(objects, fileData, i);

							object.type = LevelObjectType::TILE;
							object.tileData.material = true;
							object.x = currentX;
							object.y = fileData[i];
						}
					}


					break;

				case LevelMessages::PLANE:
				{
					auto object = NewLevelObject(objects, fileData, i);

					object.type = LevelObjectType::PLANE;
					object.planeData.w = fileData[++i];
					object.planeData.h = fileData[++i];
					object.rampData.material = currentMaterial;
					break;
				}

				case LevelMessages::RAMP:
				{
					auto object = NewLevelObject(objects, fileData, i);

					object.type = LevelObjectType::RAMP;
					object.rampData.dir = fileData[++i];
					object.rampData.material = currentMaterial;
					break;
				}

				case LevelMessages::CONNECTOR:
				{
					auto object = NewLevelObject(objects, fileData, i);

					object.type = LevelObjectType::CONNECTOR;
					object.connectorData.w = fileData[++i];
					object.connectorData.h = fileData[++i];
					break;
				}

				}
			}

		}

		return objects;
	}

}
