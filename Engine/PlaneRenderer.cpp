#include "PlaneRenderer.h"
#include "Vertex.h"


PlaneRenderer::PlaneRenderer()
{
}


PlaneRenderer::~PlaneRenderer()
{
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
}

void PlaneRenderer::Initialise()
{
	Vertex14F verts[6 + 1] =
	{
		VERT14F_TRI(Vector3(-1, 0, -1), Vector3(1, 0, -1), Vector3(-1, 0, 1), Vector2(0, 0), Vector2(1, 0), Vector2(0, 1), Vector3(0, 1, 0)),
		VERT14F_TRI(Vector3(1, 0, 1), Vector3(-1, 0, 1), Vector3(1, 0, -1), Vector2(1, 1), Vector2(0, 1), Vector2(1, 0), Vector3(0, 1, 0))
	};

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);

	for (int i = 0; i < 6; i += 3)
		Vertex14F::CalculateTangents(verts[i], verts[i + 1], verts[i + 2]);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//Position
	glEnableVertexAttribArray(1);	//UV
	glEnableVertexAttribArray(2);	//Vertex Colour
	glEnableVertexAttribArray(3);	//Tangent
	glEnableVertexAttribArray(4);	//Bitangent
	glEnableVertexAttribArray(5);	//Normal

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, pos));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, uv));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(Vertex14F) * 7)); //dumb
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, tangent));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, bitangent));
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, normal));
}
