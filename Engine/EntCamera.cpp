#include "EntCamera.hpp"
#include "GLProgram.hpp"
#include "Ray.hpp"

const EntCamera* EntCamera::_currentCamera = nullptr;

void EntCamera::Use() const
{
	_currentCamera = this;
	glViewport(0, 0, _viewport.x, _viewport.y);
	GLProgram::Current().SetMat4(DefaultUniformVars::mat4Projection, _projection);
	GLProgram::Current().SetMat4(DefaultUniformVars::mat4View, GetInverseTransformationMatrix());
}

void EntCamera::Use(int vpX, int vpY) const
{
	_currentCamera = this;
	glViewport(vpX, vpY, _viewport.x, _viewport.y);
	GLProgram::Current().SetMat4(DefaultUniformVars::mat4Projection, _projection);
	GLProgram::Current().SetMat4(DefaultUniformVars::mat4View, GetInverseTransformationMatrix());
}

void EntCamera::UpdateProjectionMatrix()
{
	switch (_type)
	{
	case EProjectionType::PERSPECTIVE:
		_projection = Matrix::Perspective(_fov, _near, _far, (float)_viewport.x / (float)_viewport.y);
		break;
	case EProjectionType::ORTHOGRAPHIC:
		_projection = Matrix::Ortho(_viewport.x / -2.f, _viewport.x / 2.f, _viewport.y / -2.f, _viewport.y / 2.f, _near, _far, _scale);
		break;
	}
}

Vector2 EntCamera::GetZPlaneDimensions() const
{
	float planeHeightAtZ = 2.f / Maths::TangentDegrees(90.f - _fov / 2.f);

	return Vector2(((float)_viewport.x / (float)_viewport.y) * planeHeightAtZ, planeHeightAtZ);
}


Ray EntCamera::ScreenCoordsToRay(const Vector2 &coords) const
{
	if (_type == EProjectionType::PERSPECTIVE)
	{
		Vector2 scale = GetZPlaneDimensions();
		Vector3 pointOnPlane = GetWorldRotation().GetQuat().Transform(Vector3(coords.x * scale.x, coords.y * scale.y, 1.f));
		pointOnPlane.Normalise();
		return Ray(GetWorldPosition(), pointOnPlane, ECollisionChannels::ALL);
	}
	else
	{
		Transform wt = GetWorldTransform();

		return Ray(GetWorldPosition() + 
			wt.GetRightVector() * ((_viewport.x * coords.x) / _scale) + 
			wt.GetUpVector() * ((_viewport.y * coords.y) / _scale), 
			wt.GetForwardVector(), ECollisionChannels::ALL);
	}
}

bool EntCamera::FrustumOverlaps(const Bounds &b) const
{
	//World to View
	Vector4 v = Vector4(b.centre) * GetInverseTransformationMatrix();
	const Vector3& v3 = reinterpret_cast<Vector3&>(v);

	float clipRadius;

	if (_type == EProjectionType::PERSPECTIVE)
	{
		//Yeah, just return true for now because the W and Z components of the clip space vector are stinky for some reason
		//todo: not this please
		return true;

		clipRadius = b.radius / (v3.Length() * Maths::TangentDegrees(_fov / 2.f));
	}
	else
		clipRadius = (b.radius * _scale) / (float)Maths::Min(_viewport.x, _viewport.y) * 2.f;

	//View To Clip
	v = v * _projection;

	//Clip to NDC
	v[0] /= v[3];
	v[1] /= v[3];
	v[2] /= v[3];

	return Collision::SphereOverlapsAABB(v3, clipRadius, Vector3(-1, -1, -1), Vector3(1, 1, 1));
}

void EntCamera::WriteData(BufferWriter<byte>& writer, NumberedSet<String>& strings) const
{
	Entity::WriteData(writer, strings);

	writer.Write_byte((byte)_type);
	writer.Write_float(_fov);
	writer.Write_float(_scale);
	writer.Write_float(_near);
	writer.Write_float(_far);
	writer.Write_uint16(_viewport.x);
	writer.Write_uint16(_viewport.y);
}

void EntCamera::ReadData(BufferReader<byte>& reader, const NumberedSet<String>& strings)
{
	Entity::ReadData(reader, strings);

	_type = EProjectionType(reader.Read_byte());
	_fov = reader.Read_float();
	_scale = reader.Read_float();
	_near = reader.Read_float();
	_far = reader.Read_float();
	_viewport.x = reader.Read_uint16();
	_viewport.y = reader.Read_uint16();
}
