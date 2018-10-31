#include "Camera.h"

void Camera::GetProperties(ObjectProperties &properties)
{
	_AddBaseProperties(properties);
}

void Camera::UpdateProjectionMatrix()
{
	switch (_type)
	{
	case ProjectionType::PERSPECTIVE:
		_projection = Matrix::Perspective(_fov, _near, _far, (float)_viewport[0] / (float)_viewport[1]);
		break;
	case ProjectionType::ORTHOGRAPHIC:
		_projection = Matrix::Ortho(_viewport[0], _viewport[1], _near, _far, _scale);
		break;
	}

}

Vector2 Camera::GetZPlaneDimensions() const
{
	float planeHeightAtZ = 2.f / Maths::TangentDegrees(90.f - _fov / 2.f);

	return Vector2(((float)_viewport[0] / (float)_viewport[1]) * planeHeightAtZ, planeHeightAtZ);
}

Vector3 Camera::ScreenCoordsToDirection(const Vector2 &coords) const
{
	Vector2 scale = GetZPlaneDimensions();
	Vector3 pointOnPlane = VectorMaths::Rotate(Vector3(coords[0] * scale[0], coords[1] * scale[1], 1.f), transform.Rotation());
	pointOnPlane.Normalise();
	return pointOnPlane;
}

//
//
// frac{y}{x}=\left(\frac{180}{k}-1\right)
// x=\frac{1}{\left(\frac{180}{k}-1\right)}
//

