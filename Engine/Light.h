#pragma once
#include "GameObject.h"
#include "Vector.h"

class CubeRenderer;

class Light : public GameObject
{
	Vector3 _colour;
	float _radius;

public:
	Light() : _radius(-1.f), _colour(1.f, 1.f, 1.f) { transform.SetScale(Vector3(.1f, .1f, .1f)); }
	~Light() {};

	inline void SetColour(const Vector3 &colour) { _colour = colour; }
	inline void SetRadius(float radius) { _radius = radius; }

	void ToShader(int glArrayIndex);

	void DebugRender(CubeRenderer &);
};

