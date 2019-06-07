#pragma once
#include "Buffer.hpp"
#include "GL.hpp"
#include "Types.hpp"
#include "Vertex.hpp"

class GLMeshRenderer
{
private:
	GLuint _vao;
	GLuint _vbo;
	GLuint _ebo; //Element buffer

	GLsizei _count; //The amount of elements/verts to draw

	GLenum _drawMode;

	void _CreateNewObjects();

public:
	GLMeshRenderer() : _vao(0), _vbo(0), _ebo(0), _count(0), _drawMode(GL_TRIANGLES) {}
	GLMeshRenderer(const GLMeshRenderer&) = delete;
	GLMeshRenderer(GLMeshRenderer&& other) noexcept : _vao(other._vao), _vbo(other._vbo), _ebo(other._ebo), _count(other._count), _drawMode(other._drawMode) 
	{
		other._vao = other._vbo = other._ebo = other._count = 0;
		other._drawMode = GL_TRIANGLES;
	}

	~GLMeshRenderer() {}

	void Create(const Vertex17F *data, GLsizei vertexCount, const uint32 *elements, GLsizei elementCount);
	void Create(const Vertex19F2I *data, GLsizei vertexCount, const uint32 *elements, GLsizei elementCount);

	inline void Create(const Vertex17F* data, GLsizei vertexCount) { Create(data, vertexCount, nullptr, 0); }
	inline void Create(const Vertex19F2I* data, GLsizei vertexCount) { Create(data, vertexCount, nullptr, 0); }

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

	inline bool operator==(const GLMeshRenderer &other) const { return _vao == other._vao; }
};
