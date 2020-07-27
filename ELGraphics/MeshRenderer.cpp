#include "MeshRenderer.hpp"

MeshRenderer::~MeshRenderer()
{
	_gl.Delete();
}

void MeshRenderer::SetRenderMode(ERenderMode mode)
{
	switch (mode)
	{
	case ERenderMode::LINES:
		_gl.SetDrawMode(GL_LINES);
		break;
	case ERenderMode::TRIANGLES:
		_gl.SetDrawMode(GL_TRIANGLES);
		break;
	}
}

void MeshRenderer::Render() const
{
	_gl.Render();
}
