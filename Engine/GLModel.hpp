#pragma once
#include "Buffer.hpp"
#include "GL.hpp"
#include "Types.hpp"
#include "Vertex.hpp"

class GLModel
{
private:
	GLuint _vao;
	GLuint _vbo;
	GLuint _ebo; //Element buffer

	GLsizei _count; //The amount of elements/verts to draw

	GLenum _drawMode;

	void _CreateNewObjects();

public:
	GLModel() : _vao(0), _vbo(0), _ebo(0), _count(0), _drawMode(GL_TRIANGLES) {}
	virtual ~GLModel() {}

	void Create(const Vertex17F *data, GLsizei vertexCount);
	void Create(const Vertex17F *data, GLsizei vertexCount, const uint32 *elements, GLsizei elementCount);

	void Create(const Vector3 *basicData, GLsizei vertexCount, const uint32 *elements, GLsizei elementCount);

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
