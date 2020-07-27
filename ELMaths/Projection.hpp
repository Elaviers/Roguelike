#pragma once
#include "Ray.hpp"
#include "Vector2.hpp"
#include "Matrix4.hpp"

struct Frustum;

class Transform;

enum class EProjectionType
{
	ORTHOGRAPHIC,
	PERSPECTIVE
};

class Projection
{
private:
	//returns the dimensions of the unit z plane
	Vector2 _GetZPlaneDimensions() const;

	Matrix4 _matrix;

	EProjectionType _type;

	Vector2T<uint16> _dimensions;

	float _near;
	float _far;

	union
	{
		struct _ortho
		{
			float scale;
		} _ortho;

		struct _persp
		{
			//degrees
			float vFOV;
		} _persp;
	};

public:
	Projection() : _type(EProjectionType::PERSPECTIVE), _dimensions(16, 9), _near(0.01f), _far(10000.f) { _persp.vFOV = 90.f; UpdateMatrix(); }
	~Projection() {}

	void SetType(EProjectionType);
	void SetDimensions(const Vector2T<uint16>& dimensions) { _dimensions = dimensions; UpdateMatrix(); }
	void SetNearFar(float n, float f) { _near = n; _far = f; UpdateMatrix(); }
	void SetPerspectiveFOV(float vFOV);
	void SetOrthographicScale(float ppu);

	const Matrix4& GetMatrix() const { return _matrix; }
	EProjectionType GetType() const { return _type; }
	const Vector2T<uint16> GetDimensions() const { return _dimensions; }
	float GetNear() const { return _near; }
	float GetFar() const { return _far; }
	float GetPerspectiveVFOV() const { return _persp.vFOV; }
	float GetOrthographicScale() const { return _ortho.scale; }

	Ray ScreenToWorld(const Transform& transform, const Vector2& coords) const;

	void ToFrustum(const Transform& transform, Frustum&) const;

	void UpdateMatrix();
};
