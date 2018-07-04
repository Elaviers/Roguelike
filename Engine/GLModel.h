#pragma once
#include "GL.h"
#include "Types.h"
#include "Vertex.h"

class GLModel
{
private:
	GLuint _vao;
	GLuint _vbo;
	GLuint _ebo; //Element buffer

	uint32 _count; //The amount of elements/verts to draw

public:
	GLModel();
	virtual ~GLModel();

	void Create(const Vertex17F *data, uint32 vertexCount);
	void Create(const Vertex17F *data, uint32 vertexCount, const uint32 *elements, uint32 elementCount);

	void Create(const Vector3 *basicData, uint32 vertexCount, const uint32 *elements, uint32 elementCount);

	virtual void Render() const
	{
		glBindVertexArray(_vao);
		if (_ebo)
			glDrawElements(GL_TRIANGLES, _count, GL_UNSIGNED_INT, nullptr);
		else
			glDrawArrays(GL_TRIANGLES, 0, _count);
	}

	inline void Delete() { glDeleteVertexArrays(1, &_vao); glDeleteBuffers(1, &_vbo); }
};

