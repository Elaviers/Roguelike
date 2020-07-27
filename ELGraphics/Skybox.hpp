#pragma once
#include "MeshManager.hpp"
#include <ELSys/GLCubemap.hpp>

class RenderQueue;

class Skybox
{
private:
	GLCubemap _cubemap;

public:
	Skybox();
	~Skybox();

	void Load(const char *faces[6], const Context&);

	void Bind(byte unit) const
	{
		_cubemap.Bind(unit);
	}

	void Render(RenderQueue& q) const;
};

