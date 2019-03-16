#include "Skybox.h"
#include "Debug.h"
#include "Engine.h"
#include "IO.h"
#include "TextureManager.h"
#include <utility> //move

Skybox::Skybox()
{
}


Skybox::~Skybox()
{
}

void Skybox::Load(const char *faces[6])
{
	Buffer<byte> faceData[6];
	uint32 faceWidth, faceHeight;

	for (int i = 0; i < 6; ++i) {
		TextureData td = IO::ReadPNGFile(CSTR(Engine::Instance().pTextureManager->GetRootPath() + faces[i]));

		if (!td.IsValid())
			return;

		if (i == 0)
		{
			faceWidth = td.width;
			faceHeight = td.height;
		}
		else if (td.width != faceWidth || td.height != faceHeight)
		{
			Debug::Error("Skybox faces must have the same dimensions");
			return;
		}

		faceData[i] = std::move(td.data);
	}

	_cubemap.Create(faceWidth, faceHeight, faceData);
}

