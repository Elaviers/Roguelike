#pragma once
#include "Buffer.h"
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

	GLenum _drawMode;

	void _CreateNewObjects();

public:
	GLModel();
	virtual ~GLModel();

	void Create(const Vertex17F *data, uint32 vertexCount);
	void Create(const Vertex17F *data, uint32 vertexCount, const uint32 *elements, uint32 elementCount);

	void Create(const Vector3 *basicData, uint32 vertexCount, const uint32 *elements, uint32 elementCount);

	inline void SetDrawMode(GLenum drawMode) { _drawMode = drawMode; }

	virtual void Render() const
	{
		glBindVertexArray(_vao);
		if (_ebo)
			glDrawElements(_drawMode, _count, GL_UNSIGNED_INT, nullptr);
		else
			glDrawArrays(_drawMode, 0, _count);
	}

	void Delete();

	inline bool IsValid() { return _vao != 0; }

	inline bool operator==(const GLModel &other) const { return _vao == other._vao; }
};
