#pragma once
#include "GizmoComponent.hpp"

class GizmoRing : public GizmoComponent
{
	float _firstAngle;

	FunctionPointer<float, const Vector3&, float> _rotate;

	bool _GetAngle(const Ray& mouseRay, float &angleOut) const;

public:
	//The function takes an axis and an angle(degrees) and returns the angle consumed
	GizmoRing(const Vector4& colour, const FunctionPointer<float, const Vector3&, float>& rotate) :
		GizmoComponent(colour),
		_firstAngle(0.f),
		_rotate(rotate) {}

	virtual ~GizmoRing() {}

	virtual void Draw() const override;
	virtual void Update(const Ray& mouseRay, float& minT) override;
};
