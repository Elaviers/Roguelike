#pragma once
#include "Entity.hpp"
#include "Collision.hpp"
#include "Types.hpp"

enum class ProjectionType
{
	PERSPECTIVE,
	ORTHOGRAPHIC
};

class EntCamera : public Entity
{
private:
	static const EntCamera* _currentCamera;

	Mat4 _projection;

	ProjectionType _type;

	float _fov;		//Used for perspective
	float _scale;	//Pixels per unit in ortho mode
	float _near;
	float _far;
	Vector<uint16, 2> _viewport;

	void UpdateProjectionMatrix();
public:
	Entity_FUNCS(EntCamera, EntityIDS::CAMERA)

	EntCamera() : _type(ProjectionType::PERSPECTIVE), _fov(90.f), _scale(1.f), _near(.001f), _far(100.f) { }
	~EntCamera() {}

	void Use();
	static const EntCamera* Current() { return _currentCamera; }

	void SetProjectionType(ProjectionType type)	{ _type = type; UpdateProjectionMatrix(); }
	void SetFOV(float fieldOfView)				{ _fov = fieldOfView; UpdateProjectionMatrix(); }
	void SetScale(float scale)					{ _scale = scale; UpdateProjectionMatrix(); }
	void SetZBounds(float n, float f)			{ _near = n; _far = f; UpdateProjectionMatrix(); }
	void SetViewport(uint16 width, uint16 height){ _viewport[0] = width; _viewport[1] = height;	UpdateProjectionMatrix(); }

	ProjectionType GetProjectionType() const		{ return _type; }
	float GetFOV() const							{ return _fov; }
	float GetFOVHorizontal() const				{ return _fov * (float)_viewport[0] / (float)_viewport[1]; }
	float GetScale() const						{ return _scale; }
	const Vector<uint16, 2>& GetViewport() const	{ return _viewport; }
	float GetNear() const						{ return _near; }
	float GetFar() const							{ return _far; }

	const Mat4& GetProjectionMatrix() const		{ return _projection; }

	bool FrustumOverlaps(const Bounds &b) const;

	//coords are between -.5f(top-left) and .5f(bottom-right)
	Ray ScreenCoordsToRay(const Vector2 &coords) const;

	Vector2 GetZPlaneDimensions() const;

	//todo : implement this xd
	//virtual const PropertyCollection& GetProperties() override;

	//File IO
	virtual void WriteData(BufferWriter<byte>&, NumberedSet<String>& strings) const override;
	virtual void ReadData(BufferReader<byte>&, const NumberedSet<String>& strings) override;
};
