#pragma once
#include "Renderer.h"
#include "GL.h"
#include "Types.h"
#include "Vertex.h"

class ModelRenderer : public Renderer
{
private:
	GLuint _vao;
	GLuint _vbo;
	GLuint _ebo; //Element buffer

	uint32 _elementCount;

public:
	ModelRenderer();
	virtual ~ModelRenderer();

	void Create(const Vertex14F *data, uint32 vertexCount, const uint32 *elements, uint32 elementCount);
	
	virtual void Render() const
	{
		glBindVertexArray(_vao); 
		glDrawElements(GL_TRIANGLES, _elementCount, GL_UNSIGNED_INT, nullptr);
	}

	inline void Delete() { glDeleteVertexArrays(1, &_vao); glDeleteBuffers(1, &_vbo); }
};
