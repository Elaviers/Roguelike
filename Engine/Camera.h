#pragma once
#include "GameObject.h"
#include "Types.h"

enum class ProjectionType
{
	PERSPECTIVE,
	ORTHOGRAPHIC
};

class Camera : public GameObject
{
private:
	Mat4 _projection;

	ProjectionType _type;

	float _fov;		//Used for perspective
	float _scale;	//Pixels per unit in ortho mode
	float _near;
	float _far;
	Vector<uint16, 2> _viewport;

	void UpdateProjectionMatrix();
public:
	Camera() : _type(ProjectionType::PERSPECTIVE), _fov(90.f), _scale(32.f), _near(.001f), _far(100.f) { }
	~Camera() {}

	inline void SetProectionType(ProjectionType type)	{ _type = type; UpdateProjectionMatrix(); }
	inline void SetFOV(float fieldOfView)				{ _fov = fieldOfView; UpdateProjectionMatrix(); }
	inline void SetScale(float scale)					{ _scale = scale; UpdateProjectionMatrix(); }
	inline void SetZBounds(float n, float f)			{ _near = n; _far = f; UpdateProjectionMatrix(); }
	inline void SetViewport(uint16 width, uint16 height){ _viewport[0] = width; _viewport[1] = height;	UpdateProjectionMatrix(); }

	inline ProjectionType GetProjectionType() const		{ return _type; }
	inline float GetFOV() const							{ return _fov; }
	inline float GetFOVHorizontal() const				{ return _fov * (float)_viewport[0] / (float)_viewport[1]; }
	inline float GetScale() const						{ return _scale; }
	inline Vector<uint16, 2> GetViewport() const { return _viewport; }

	inline Mat4 GetProjectionMatrix() const				{ return _projection; }

	Vector2 GetZPlaneDimensions() const;

	virtual void GetProperties(ObjectProperties&) override;
};
