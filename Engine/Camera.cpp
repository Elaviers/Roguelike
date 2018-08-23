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
