#include "CubeRenderer.h"
#include "Utilities.h"
#include "Vertex.h"

CubeRenderer::CubeRenderer() : _vaos(), _vbos()
{
}


CubeRenderer::~CubeRenderer()
{
	glDeleteVertexArrays(2, _vaos);
	glDeleteBuffers(2, _vbos);
}

void BindGLVertexAttributes()
{
	glEnableVertexAttribArray(0);	//Position
	glEnableVertexAttribArray(1);	//UV
	glEnableVertexAttribArray(2);	//Vertex Colour
	glEnableVertexAttribArray(3);	//Tangent
	glEnableVertexAttribArray(4);	//Bitangent
	glEnableVertexAttribArray(5);	//Normal

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, pos));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, uv));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(Vertex14F) * 37)); //Why am I not fixing this
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, tangent));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, bitangent));
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14F), (GLvoid*)offsetof(Vertex14F, normal));
}

void CubeRenderer::Initialise() {
	Vertex14F verts[36 + 1] =
	{
		//Bottom
		VERT14F_TRI(Vector3(1, -1, -1),		Vector3(-1, -1, -1),	Vector3(1, -1, 1),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(0, -1, 0)),
		VERT14F_TRI(Vector3(-1, -1, 1),		Vector3(1, -1, 1),		Vector3(-1, -1, -1),Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(0, -1, 0)),

		//Top
		VERT14F_TRI(Vector3(-1, 1, -1),		Vector3(1, 1, -1),		Vector3(-1, 1, 1),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(0, 1, 0)),
		VERT14F_TRI(Vector3(1, 1, 1),		Vector3(-1, 1, 1),		Vector3(1, 1, -1),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(0, 1, 0)),

		//Front
		VERT14F_TRI(Vector3(-1, -1, -1),	Vector3(1, -1, -1),		Vector3(-1, 1, -1),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(0, 0, -1)),
		VERT14F_TRI(Vector3(1, 1, -1),		Vector3(-1, 1, -1),		Vector3(1, -1, -1),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(0, 0, -1)),

		//Right
		VERT14F_TRI(Vector3(1, -1, -1),		Vector3(1, -1, 1),		Vector3(1, 1, -1),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(1, 0, 0)),
		VERT14F_TRI(Vector3(1, 1, 1),		Vector3(1, 1, -1),		Vector3(1, -1, 1),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(1, 0, 0)),

		//Back
		VERT14F_TRI(Vector3(1, -1, 1),		Vector3(-1, -1, 1),		Vector3(1, 1, 1),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(0, 0, 1)),
		VERT14F_TRI(Vector3(-1, 1, 1),		Vector3(1, 1, 1),		Vector3(-1, -1, 1),	Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(0, 0, 1)),

		//Left
		VERT14F_TRI(Vector3(-1, -1, 1),		Vector3(-1, -1, -1),	Vector3(-1, 1, 1),	Vector2(0.f, 0.f),	Vector2(1.f, 0.f),	Vector2(0.f, 1.f),	Vector3(-1, 0, 0)),
		VERT14F_TRI(Vector3(-1, 1, -1),		Vector3(-1, 1, 1),		Vector3(-1, -1, -1),Vector2(1.f, 1.f),	Vector2(0.f, 1.f),	Vector2(1.f, 0.f),	Vector3(-1, 0, 0)),
	};

	for (int i = 0; i < 36; i += 3)
		Vertex14F::CalculateTangents(verts[i], verts[i + 1], verts[i + 2]);

	glGenVertexArrays(2, _vaos);
	glGenBuffers(2, _vbos);

	glBindVertexArray(_vaos[0]);
	glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts, GL_STATIC_DRAW);
	BindGLVertexAttributes();

	//Inverse
	for (int i = 0; i < 36; ++i)
		verts[i].normal *= -1;

	for (int i = 0; i < 36; i += 3) {
		Utilities::Swap(verts[i + 1], verts[i + 2]);
		Vertex14F::CalculateTangents(verts[i], verts[i + 1], verts[i + 2]);
	}

	glBindVertexArray(_vaos[1]);
	glBindBuffer(GL_ARRAY_BUFFER, _vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts, GL_STATIC_DRAW);
	BindGLVertexAttributes();
}
