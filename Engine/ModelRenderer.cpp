#include "ModelRenderer.h"



ModelRenderer::ModelRenderer() : _vao(0), _vbo(0), _ebo(0)
{
}


ModelRenderer::~ModelRenderer()
{
}

void ModelRenderer::Create(const Vertex14F *data, uint32 vertexCount, const uint32 *elements, uint32 elementCount)
{
	if (_vao || _vbo || _ebo)
	{
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_vbo);
		glDeleteBuffers(1, &_ebo);
	}

	_elementCount = elementCount;

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ebo);

	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex14F), data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _elementCount * sizeof(uint32), elements, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//Position
	glEnableVertexAttribArray(1);	//UV
	glEnableVertexAttribArray(2);	//Vertex Colour
	glEnableVertexAttribArray(3);	//Tangent
	glEnableVertexAttribArray(4);	//Bitangent
	glEnableVertexAttribArray(5);	//Normal
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, pos));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, uv));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, tangent));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, bitangent));
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, normal));
}
