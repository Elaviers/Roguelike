#pragma once
#include "Renderer.h"
#include "GL.h"

class CubeRenderer : public Renderer
{
private:
	GLuint _vaos[2], _vbos[2];		//0 : cube, 1 : cube, inverted normals

public:
	CubeRenderer();
	virtual ~CubeRenderer();

	void Initialise();

	virtual void Render() const
	{
		glBindVertexArray(_vaos[0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void RenderInverse() const
	{
		glBindVertexArray(_vaos[1]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
};
