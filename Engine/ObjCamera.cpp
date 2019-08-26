#include "ObjCamera.hpp"
#include "GLProgram.hpp"
#include "Ray.hpp"

const ObjCamera* ObjCamera::_currentCamera = nullptr;

void ObjCamera::Use()
{
	_currentCamera = this;
	GLProgram::Current().SetMat4(DefaultUniformVars::mat4Projection, _projection);
	GLProgram::Current().SetMat4(DefaultUniformVars::mat4View, GetInverseTransformationMatrix());
}

void ObjCamera::GetCvars(CvarMap &properties)
{
	_AddBaseCvars(properties);
}

void ObjCamera::UpdateProjectionMatrix()
{
	switch (_type)
	{
	case ProjectionType::PERSPECTIVE:
		_projection = Matrix::Perspective(_fov, _near, _far, (float)_viewport[0] / (float)_viewport[1]);
		break;
	case ProjectionType::ORTHOGRAPHIC:
		_projection = Matrix::Ortho(_viewport[0] / -2.f, _viewport[0] / 2.f, _viewport[1] / -2.f, _viewport[1] / 2.f, _near, _far, _scale);
		break;
	}
}

Vector2 ObjCamera::GetZPlaneDimensions() const
{
	float planeHeightAtZ = 2.f / Maths::TangentDegrees(90.f - _fov / 2.f);

	return Vector2(((float)_viewport[0] / (float)_viewport[1]) * planeHeightAtZ, planeHeightAtZ);
}


Ray ObjCamera::ScreenCoordsToRay(const Vector2 &coords) const
{
	if (_type == ProjectionType::PERSPECTIVE)
	{
		Vector2 scale = GetZPlaneDimensions();
		Vector3 pointOnPlane = GetWorldRotation().GetQuat().Transform(Vector3(coords[0] * scale[0], coords[1] * scale[1], 1.f));
		pointOnPlane.Normalise();
		return Ray(GetWorldPosition(), pointOnPlane, COLL_ALL_CHANNELS);
	}
	else
	{
		Transform wt = GetWorldTransform();

		return Ray(GetWorldPosition() + 
			wt.GetRightVector() * ((_viewport[0] * coords[0]) / _scale) + 
			wt.GetUpVector() * ((_viewport[1] * coords[1]) / _scale), 
			wt.GetForwardVector(), COLL_ALL_CHANNELS);
	}
}

bool ObjCamera::FrustumOverlaps(const Bounds &b) const
{
	//World to View
	Vector4 v = Vector4(b.centre) * GetInverseTransformationMatrix();
	const Vector3& v3 = reinterpret_cast<Vector3&>(v);

	float clipRadius;

	if (_type == ProjectionType::PERSPECTIVE)
	{
		//Yeah, just return true for now because the W and Z components of the clip space vector are stinky for some reason
		//todo: not this please
		return true;

		clipRadius = b.radius / (v3.Length() * Maths::TangentDegrees(_fov / 2.f));
	}
	else
		clipRadius = (b.radius * _scale) / (float)Utilities::Min(_viewport[0], _viewport[1]) * 2.f;

	Debug::PrintLine(CSTR(String::FromVector3(v3) + ", " + String::FromFloat(v[3]) + " (" + String::FromFloat(clipRadius) + ")"));
	
	//View To Clip
	v = v * _projection;

	//Clip to NDC
	v[0] /= v[3];
	v[1] /= v[3];
	v[2] /= v[3];

	return Collision::SphereOverlapsAABB(v3, clipRadius, Vector3(-1, -1, -1), Vector3(1, 1, 1));
}

void ObjCamera::WriteData(BufferWriter<byte>& writer, NumberedSet<String>& strings) const
{
	GameObject::WriteData(writer, strings);

	writer.Write_byte((byte)_type);
	writer.Write_float(_fov);
	writer.Write_float(_scale);
	writer.Write_float(_near);
	writer.Write_float(_far);
	writer.Write_uint16(_viewport[0]);
	writer.Write_uint16(_viewport[1]);
}

void ObjCamera::ReadData(BufferReader<byte>& reader, const NumberedSet<String>& strings)
{
	GameObject::ReadData(reader, strings);

	_type = ProjectionType(reader.Read_byte());
	_fov = reader.Read_float();
	_scale = reader.Read_float();
	_near = reader.Read_float();
	_far = reader.Read_float();
	_viewport[0] = reader.Read_uint16();
	_viewport[1] = reader.Read_uint16();
}
