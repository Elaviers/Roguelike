#include "Camera.h"

void Camera::UpdateProjectionMatrix()
{
	switch (_type)
	{
	case ProjectionType::PERSPECTIVE:
		_projection = Matrix::Perspective(_fieldOfView, _near, _far, _aspectRatio);
		break;
	case ProjectionType::ORTHOGRAPHIC:
		_projection = Matrix::Ortho(20, 20, _near, _far);
		_projection = Matrix::Perspective(_fieldOfView, _near, _far, _aspectRatio);
		break;
	}

	
}
