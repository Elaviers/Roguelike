#pragma once
#include "GameObject.h"

class Camera : public GameObject
{
private:
	Mat4 _projection;

	enum class ProjectionType
	{
		PERSPECTIVE,
		ORTHOGRAPHIC
	} _type;

	float _fieldOfView;
	float _near;
	float _far;
	float _aspectRatio;

	void UpdateProjectionMatrix();
public:
	Camera() : _fieldOfView(90.f), _near(.001f), _far(100.f) {}
	~Camera() {}

	inline void SetFOV(float fieldOfView)			{ _fieldOfView = fieldOfView;	UpdateProjectionMatrix(); }
	inline void SetZBounds(float n, float f) { _near = n; _far = f; UpdateProjectionMatrix(); }
	inline void SetAspectRatio(float aspectRatio)	{ _aspectRatio = aspectRatio;	UpdateProjectionMatrix(); }

	inline Mat4 GetProjectionMatrix() { return _projection; }
};
