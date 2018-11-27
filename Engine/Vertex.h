#pragma once
#include "Vector.h"

struct Vertex17F
{
	Vector3 pos;
	Vector2 uvOffset;
	Vector3 colour = Vector3(1.f, 1.f, 1.f);

	Vector3 tangent;
	Vector3 bitangent;
	Vector3 normal;

	static void CalculateTangents(Vertex17F &vertex1, Vertex17F &vertex2, Vertex17F &vertex3);
};

#define VERT14F_TRI(POS1, POS2, POS3, UV1, UV2, UV3, NORMAL) \
		{POS1, UV1, Vector3(1.f, 1.f, 1.f), Vector3(), Vector3(), NORMAL}, {POS2, UV2, Vector3(1.f, 1.f, 1.f), Vector3(), Vector3(), NORMAL}, {POS3, UV3, Vector3(1.f, 1.f, 1.f), Vector3(), Vector3(), NORMAL}
