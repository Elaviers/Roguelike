#pragma once
#include "GameObject.h"
#include "Vector.h"

class ModelManager;

class ObjLight : public GameObject
{
	Vector3 _colour;
	float _radius;

public:
	GAMEOBJECT_FUNCS(ObjLight)

	ObjLight() : GameObject(FLAG_SAVEABLE), _radius(-1.f), _colour(1.f, 1.f, 1.f) { transform.SetScale(Vector3(.1f, .1f, .1f)); }
	~ObjLight() {};

	static bool drawLightSources;

	inline void SetColour(const Vector3 &colour) { _colour = colour; }
	inline void SetRadius(float radius) { _radius = radius; }

	void ToShader(int glArrayIndex);

	void Render() const override;

	virtual void GetCvars(CvarMap&) override;
};
