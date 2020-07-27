#include "Skybox.hpp"
#include "IO.hpp"
#include "RenderCommand.hpp"
#include "RenderQueue.hpp"
#include "TextureManager.hpp"
#include <ELCore/Context.hpp>
#include <ELSys/Debug.hpp>
#include <utility> //move

Skybox::Skybox()
{
}


Skybox::~Skybox()
{
}

void Skybox::Load(const char *faceFilenames[6], const Context& context)
{
	TextureManager* textureManager = context.GetPtr<TextureManager>();
	if (textureManager == nullptr) Debug::Error("Skybox::Load called without textureManager in context");

	Texture* textures[6];
	TextureData faces[6] = {};
	uint32 faceWidth, faceHeight;

	for (int i = 0; i < 6; ++i) {
		//todo- this is dumb!!!!!!!!!!
		Texture *tex = IO::ReadPNGFile(CSTR(textureManager->GetRootPath(), faceFilenames[i]));
		textures[i] = tex;

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

		faces[i].width = tex->GetWidth();
		faces[i].height = tex->GetHeight();
		faces[i].data = tex->GetData().Data();
	}

	_cubemap.Create(faces);

	for (int i = 0; i < 6; ++i)
		delete textures[i];
}

void Skybox::Render(RenderQueue& q) const
{
	{
		RenderEntry& e = q.NewDynamicEntry(ERenderChannels::SKY);
		e.AddSetCubemap(_cubemap, 0);
		e.AddCommand(RCMDRenderMesh::INV_CUBE);
	}

	{
		RenderEntry& e = q.NewDynamicEntry(ERenderChannels::SURFACE, -1);
		e.AddSetCubemap(_cubemap, 100);
	}
}
