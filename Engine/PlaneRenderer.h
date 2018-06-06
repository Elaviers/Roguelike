#pragma once
#include "Renderer.h"
#include "GL.h"

class PlaneRenderer : public Renderer
{
private:
	GLuint _vao, _vbo;

public:
	PlaneRenderer();
	virtual ~PlaneRenderer();

	void Initialise();

	virtual void Render()
	{
		glBindVertexArray(_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

};

