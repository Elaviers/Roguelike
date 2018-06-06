#pragma once
#include "Vector.h"

struct Vertex6F
{
	float pos[3];
	float normal[3];
};

struct Vertex8F
{
	float pos[3];
	float normal[3];
	float uv[2];
};

struct Vertex14F
{
	Vector3 pos;
	Vector2 uv;
	Vector3 tangent;
	Vector3 bitangent;
	Vector3 normal;

	static void CalculateTangents(Vertex14F &vertex1, Vertex14F &vertex2, Vertex14F &vertex3);
};

#define VERT14F_TRI(POS1, POS2, POS3, UV1, UV2, UV3, NORMAL) \
		{POS1, UV1, Vector3(), Vector3(), NORMAL}, {POS2, UV2, Vector3(), Vector3(), NORMAL}, {POS3, UV3, Vector3(), Vector3(), NORMAL}
