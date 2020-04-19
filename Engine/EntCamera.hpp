#pragma once
#include "Entity.hpp"
#include "Collision.hpp"
#include "Types.hpp"

enum class EProjectionType
{
	PERSPECTIVE,
	ORTHOGRAPHIC
};

class EntCamera : public Entity
{
private:
	static const EntCamera* _currentCamera;

	Mat4 _projection;

	EProjectionType _type;

	float _fov;		//Used for perspective
	float _scale;	//Pixels per unit in ortho mode
	float _near;
	float _far;
	Vector2T<uint16> _viewport;

	void UpdateProjectionMatrix();
public:
	Entity_FUNCS(EntCamera, EEntityID::CAMERA)

	EntCamera() : _type(EProjectionType::PERSPECTIVE), _fov(90.f), _scale(1.f), _near(.001f), _far(100.f) { }
	~EntCamera() {}

	void Use() const;
	void Use(int vpX, int vpY) const;
	static const EntCamera* Current() { return _currentCamera; }

	void SetProjectionType(EProjectionType type)	{ _type = type; UpdateProjectionMatrix(); }
	void SetPerspFOV(float fieldOfView)				{ _fov = fieldOfView; UpdateProjectionMatrix(); }
	void SetOrthoPixelsPerUnit(float scale)			{ _scale = scale; UpdateProjectionMatrix(); }
	void SetZBounds(float n, float f)				{ _near = n; _far = f; UpdateProjectionMatrix(); }
	void SetViewport(uint16 width, uint16 height)	{ _viewport.x = width; _viewport.y = height; UpdateProjectionMatrix(); }

	EProjectionType GetProjectionType() const		{ return _type; }
	float GetPerspFOV() const						{ return _fov; }
	float GetPerspFOVHorizontal() const				{ return _fov * (float)_viewport.x / (float)_viewport.y; }
	float GetOrthoPixelsPerUnit() const				{ return _scale; }
	const Vector2T<uint16>& GetViewport() const		{ return _viewport; }
	float GetNear() const							{ return _near; }
	float GetFar() const							{ return _far; }

	const Mat4& GetProjectionMatrix() const			{ return _projection; }

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
