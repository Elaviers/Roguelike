#pragma once
#include <ELSys/GLMeshRenderer.hpp>

enum class ERenderMode
{
	LINES,
	TRIANGLES
};

class MeshRenderer
{
private:
	GLMeshRenderer _gl;

public:
	MeshRenderer() {}
	MeshRenderer(const MeshRenderer&) = delete;
	~MeshRenderer();

	void SetRenderMode(ERenderMode mode);

	//todo: don't expose this
	GLMeshRenderer& GL() { return _gl; }

	void Render() const;
};
