#include "GLMeshRenderer.hpp"

inline void SetGLAttribsStatic()
{
	glEnableVertexAttribArray(0);	//Position
	glEnableVertexAttribArray(1);	//UV
	glEnableVertexAttribArray(2);	//Vertex Colour
	glEnableVertexAttribArray(3);	//Tangent
	glEnableVertexAttribArray(4);	//Bitangent
	glEnableVertexAttribArray(5);	//Normal

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex17F), (GLvoid*)offsetof(Vertex17F, pos));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex17F), (GLvoid*)offsetof(Vertex17F, uv));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex17F), (GLvoid*)offsetof(Vertex17F, colour));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex17F), (GLvoid*)offsetof(Vertex17F, tangent));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex17F), (GLvoid*)offsetof(Vertex17F, bitangent));
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex17F), (GLvoid*)offsetof(Vertex17F, normal));
}

inline void SetGLAttribsSkeletal()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSkeletal), (GLvoid*)offsetof(VertexSkeletal, pos));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexSkeletal), (GLvoid*)offsetof(VertexSkeletal, uv));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSkeletal), (GLvoid*)offsetof(VertexSkeletal, colour));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSkeletal), (GLvoid*)offsetof(VertexSkeletal, tangent));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSkeletal), (GLvoid*)offsetof(VertexSkeletal, bitangent));
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSkeletal), (GLvoid*)offsetof(VertexSkeletal, normal));

	glVertexAttribPointer(6, VertexSkeletal::BONE_COUNT, GL_UNSIGNED_INT,	GL_FALSE, sizeof(VertexSkeletal), (GLvoid*)offsetof(VertexSkeletal, boneIndices));
	glVertexAttribPointer(7, VertexSkeletal::BONE_COUNT, GL_FLOAT,			GL_FALSE, sizeof(VertexSkeletal), (GLvoid*)offsetof(VertexSkeletal, boneWeights));
}

//Creates a VAO/VBO and gets rid of any old objects
void GLMeshRenderer::_CreateNewObjects()
{
	if (_vao || _vbo || _ebo)
	{
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_vbo);
		glDeleteBuffers(1, &_ebo);
	}

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
}

void GLMeshRenderer::Create(const Vertex17F *data, GLsizei vertexCount, const uint32 *elements, GLsizei elementCount)
{
	_CreateNewObjects();

	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex17F), data, GL_STATIC_DRAW);

	if (elementCount > 0)
	{
		_count = elementCount;

		glGenBuffers(1, &_ebo);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _count * sizeof(uint32), elements, GL_STATIC_DRAW);
	}
	else
		_count = vertexCount;

	SetGLAttribsStatic();
}

void GLMeshRenderer::Create(const VertexSkeletal* data, GLsizei vertexCount, const uint32* elements, GLsizei elementCount)
{
	_CreateNewObjects();

	_count = elementCount;

	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(VertexSkeletal), data, GL_STATIC_DRAW);

	if (_count > 0)
	{
		glGenBuffers(1, &_ebo);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _count * sizeof(uint32), elements, GL_STATIC_DRAW);
	}

	SetGLAttribsSkeletal();
}

void GLMeshRenderer::Create(const Vector3 *basicData, GLsizei vertexCount, const uint32 *elements, GLsizei elementCount)
{
	_CreateNewObjects();
	glGenBuffers(1, &_ebo);

	_count = elementCount;


	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex17F), basicData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _count * sizeof(uint32), elements, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
}

void GLMeshRenderer::Delete()
{
	_count = 0;

	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);

	_vao = _vbo = 0;

	if (_ebo)
	{
		glDeleteBuffers(1, &_ebo);
		_ebo = 0;
	}
}
