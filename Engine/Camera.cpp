#include "Camera.h"
#include "Ray.h"

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
	
	UpdateWorldToClip();
}

Vector2 Camera::GetZPlaneDimensions() const
{
	float planeHeightAtZ = 2.f / Maths::TangentDegrees(90.f - _fov / 2.f);

	return Vector2(((float)_viewport[0] / (float)_viewport[1]) * planeHeightAtZ, planeHeightAtZ);
}


Ray Camera::ScreenCoordsToRay(const Vector2 &coords) const
{
	if (_type == ProjectionType::PERSPECTIVE)
	{
		Vector2 scale = GetZPlaneDimensions();
		Vector3 pointOnPlane = VectorMaths::Rotate(Vector3(coords[0] * scale[0], coords[1] * scale[1], 1.f), transform.Rotation());
		pointOnPlane.Normalise();
		return Ray(transform.Position(), pointOnPlane);
	}
	else
	{
		return Ray(transform.Position() + 
			transform.GetRightVector() * ((_viewport[0] * coords[0]) / _scale) + 
			transform.GetUpVector() * ((_viewport[1] * coords[1]) / _scale), 
			
			transform.GetForwardVector());
	}
}

bool Camera::IsInView(const Bounds &b) const
{
	Vector4 viewCentre = Vector4(b.centre) * _worldToClip;
	float viewRadius = b.radius / Maths::TangentDegrees(_fov / 2.f);

	const float w = viewCentre[3];
	const float nw = -viewCentre[3];
	return Collision::SphereOverlapsAABB(Vector3(viewCentre[0], viewCentre[1], viewCentre[2]), viewRadius, Vector3(nw, nw, nw), Vector3(w, w, w));
}

//
//
// frac{y}{x}=\left(\frac{180}{k}-1\right)
// x=\frac{1}{\left(\frac{180}{k}-1\right)}
//
