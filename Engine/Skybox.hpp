#pragma once
#include "GLCubemap.hpp"
#include "ModelManager.hpp"

class Skybox
{
private:
	GLCubemap _cubemap;

public:
	Skybox();
	~Skybox();

	void Load(const char *faces[6]);

	inline void Bind(byte unit) const
	{
		_cubemap.Bind(unit);
	}

	inline void Render(const ModelManager &cubeRenderer) const
	{
		Bind(0);
		cubeRenderer.InverseCube().Render();
	}
};
