#include "Skybox.hpp"
#include "Debug.hpp"
#include "Engine.hpp"
#include "IO.hpp"
#include "TextureManager.hpp"
#include <utility> //move

Skybox::Skybox()
{
}


Skybox::~Skybox()
{
}

void Skybox::Load(const char *faceFilenames[6])
{
	Texture* faces[6] = {};
	uint32 faceWidth, faceHeight;

	for (int i = 0; i < 6; ++i) {
		Texture *tex = IO::ReadPNGFile(CSTR(Engine::Instance().pTextureManager->GetRootPath(), faceFilenames[i]));

		if (!tex->IsValid())
			return;

		if (i == 0)
		{
			faceWidth = tex->GetWidth();
			faceHeight = tex->GetHeight();
		}
		else if (tex->GetWidth() != faceWidth || tex->GetHeight() != faceHeight)
		{
			Debug::Error("Skybox faces must have the same dimensions");
			break;
		}

		faces[i] = tex;
	}

	_cubemap.Create(faces);

	for (int i = 0; i < 6; ++i)
		delete faces[i];
}

