#include "Projection.hpp"
#include "Frustum.hpp"
#include <ELMaths/Matrix4.hpp>
#include <ELMaths/Transform.hpp>

void Projection::UpdateMatrix()
{
	if (_type == EProjectionType::PERSPECTIVE)
	{
		_matrix = Matrix4::ProjectionPerspective(_persp.vFOV * Maths::RADS_PER_DEG, _near, _far, (float)_dimensions.x / (float)_dimensions.y);
	}
	else
	{
		float hx = _dimensions.x / 2.f;
		float hy = _dimensions.y / 2.f;

		_matrix = Matrix4::ProjectionOrtho(-hx, hx,-hy, hy, _near, _far, _ortho.scale);
	}
}

Vector2 Projection::_GetZPlaneDimensions() const
{
	if (_type == EProjectionType::ORTHOGRAPHIC)
		return _dimensions;

	float planeHeightAtZ = 2.f / Maths::TangentDegrees(90.f - _persp.vFOV / 2.f);

	return Vector2(((float)_dimensions.x / (float)_dimensions.y) * planeHeightAtZ, planeHeightAtZ);
}

void Projection::SetType(EProjectionType type)
{
	if (_type != type)
	{
		_type = type;

		if (_type == EProjectionType::ORTHOGRAPHIC)
			_ortho.scale = 1.f;
		else
			_persp.vFOV = 90.f;


		UpdateMatrix();
	}
}

void Projection::SetPerspectiveFOV(float vFOV)
{
	Debug::Assert(_type == EProjectionType::PERSPECTIVE, "SetPerspectiveFOV not called on a perspective projection! That won't do much!");

	if (_type == EProjectionType::PERSPECTIVE)
	{
		_persp.vFOV = vFOV;
		UpdateMatrix();
	}
}

void Projection::SetOrthographicScale(float ppu)
{
	Debug::Assert(_type == EProjectionType::ORTHOGRAPHIC, "SetOrthographicScale not called on an orthographic projection! That won't do much!");

	if (_type == EProjectionType::ORTHOGRAPHIC)
	{
		_ortho.scale = ppu;
		UpdateMatrix();
	}
}

Ray Projection::ScreenToWorld(const Transform& transform, const Vector2& coords) const
{
	if (_type == EProjectionType::PERSPECTIVE)
	{
		Vector2 scale = _GetZPlaneDimensions();
		Vector3 pointOnPlane = transform.GetRotation().GetQuat().Transform(Vector3(coords.x * scale.x, coords.y * scale.y, 1.f));
		pointOnPlane.Normalise();
		return Ray(transform.GetPosition(), pointOnPlane);
	}
	else
	{
		return Ray(transform.GetPosition() +
			transform.GetRightVector() * ((_dimensions.x * coords.x) / _ortho.scale) +
			transform.GetUpVector() * ((_dimensions.y * coords.y) / _ortho.scale),
			transform.GetForwardVector());
	}
}

Vector3 RayPlaneIntersection(const Vector3& start, const Vector3& dir, const Vector3& planePoint, const Vector3& planeNormal)
{
	float denominator = dir.Dot(planeNormal);
	if (denominator == 0.f) return 0.f;	//Axes are perpendicular

	float t = (planePoint - start).Dot(planeNormal) / denominator;
	return start + dir * t;
}

void Projection::ToFrustum(const Transform& transform, Frustum& f) const
{
	f.position = transform.GetPosition();
	f.normalFar = transform.GetForwardVector();
	f.normalNear = f.normalFar * -1.f;
	
	if (_type == EProjectionType::PERSPECTIVE)
	{
		Vector2 zp = _GetZPlaneDimensions();
		Vector3 topLeft = transform.GetRotation().GetQuat().Transform(Vector3(-zp.x, zp.y, 1.f));
		Vector3 topRight = transform.GetRotation().GetQuat().Transform(Vector3(zp.x, zp.y, 1.f));
		Vector3 btmLeft = transform.GetRotation().GetQuat().Transform(Vector3(-zp.x, -zp.y, 1.f));
		Vector3 btmRight = transform.GetRotation().GetQuat().Transform(Vector3(zp.x, -zp.y, 1.f));
		
		Vector3 cNear = f.position + f.normalFar * _near;
		Vector3 cFar = f.position + f.normalFar * _far;

		f.normalTop = topLeft.Cross(topRight).Normalised();
		f.normalRight = topRight.Cross(btmRight).Normalised();
		f.normalBottom = btmRight.Cross(btmLeft).Normalised();
		f.normalLeft = btmLeft.Cross(topLeft).Normalised();
		f.nearBtmLeft = RayPlaneIntersection(f.position, btmLeft, cNear, f.normalNear);
		f.nearBtmRight = RayPlaneIntersection(f.position, btmRight, cNear, f.normalNear);
		f.nearTopLeft = RayPlaneIntersection(f.position, topLeft, cNear, f.normalNear);
		f.nearTopRight = RayPlaneIntersection(f.position, topRight, cNear, f.normalNear);
		f.farBtmLeft = RayPlaneIntersection(f.position, btmLeft, cFar, f.normalFar);
		f.farBtmRight = RayPlaneIntersection(f.position, btmRight, cFar, f.normalFar);
		f.farTopLeft = RayPlaneIntersection(f.position, topLeft, cFar, f.normalFar);
		f.farTopRight = RayPlaneIntersection(f.position, topRight, cFar, f.normalFar);
	}
	else if (_type == EProjectionType::ORTHOGRAPHIC)
	{
		f.normalRight = transform.GetRightVector();
		f.normalLeft = f.normalRight * -1.f;
		f.normalTop = transform.GetUpVector();
		f.normalBottom = f.normalTop * -1.f;

		Vector3 deltaR = f.normalRight * (_dimensions.x / _ortho.scale / 2.f);
		Vector3 deltaU = f.normalTop * (_dimensions.y / _ortho.scale / 2.f);
		
		Vector3 nearCtr = f.position + f.normalFar * _near;
		Vector3 nearBtm = nearCtr - deltaU;
		Vector3 nearTop = nearCtr + deltaU;
		Vector3 farCtr = f.position + f.normalFar * _far;
		Vector3 farBtm = farCtr - deltaU;
		Vector3 farTop = farCtr + deltaU;

		f.nearBtmLeft = nearBtm - deltaR;
		f.nearBtmRight = nearBtm + deltaR;
		f.nearTopLeft = nearTop - deltaR;
		f.nearTopRight = nearTop + deltaR;
		f.farBtmLeft = farBtm - deltaR;
		f.farBtmRight = farBtm + deltaR;
		f.farTopLeft = farTop - deltaR;
		f.farTopRight = farTop + deltaR;
	}
}
