#include "EntCamera.hpp"
#include <ELSys/GLProgram.hpp>

const EntCamera* EntCamera::_currentCamera = nullptr;

void EntCamera::Use() const
{
	_currentCamera = this;
	glViewport(0, 0, _projection.GetDimensions().x, _projection.GetDimensions().y);
	GLProgram::Current().SetMatrix4(DefaultUniformVars::mat4Projection, _projection.GetMatrix());
	GLProgram::Current().SetMatrix4(DefaultUniformVars::mat4View, GetInverseTransformationMatrix());
}

void EntCamera::Use(int vpX, int vpY) const
{
	_currentCamera = this;
	glViewport(vpX, vpY, _projection.GetDimensions().x, _projection.GetDimensions().y);
	GLProgram::Current().SetMatrix4(DefaultUniformVars::mat4Projection, _projection.GetMatrix());
	GLProgram::Current().SetMatrix4(DefaultUniformVars::mat4View, GetInverseTransformationMatrix());
}

const PropertyCollection& EntCamera::GetProperties()
{
	static PropertyCollection properties;
	DO_ONCE_BEGIN;
	_AddBaseProperties(properties);

	DO_ONCE_END;

	return properties;
}

void EntCamera::WriteData(ByteWriter& writer, NumberedSet<String>& strings, const Context& ctx) const
{
	Entity::WriteData(writer, strings, ctx);

	writer.Write_byte((byte)_projection.GetType());

	if (_projection.GetType() == EProjectionType::PERSPECTIVE)
		writer.Write_float(_projection.GetPerspectiveVFOV());
	else
		writer.Write_float(_projection.GetOrthographicScale());

	writer.Write_float(_projection.GetNear());
	writer.Write_float(_projection.GetFar());
	writer.Write_uint16(_projection.GetDimensions().x);
	writer.Write_uint16(_projection.GetDimensions().y);
}

void EntCamera::ReadData(ByteReader& reader, const NumberedSet<String>& strings, const Context& ctx)
{
	Entity::ReadData(reader, strings, ctx);

	_projection.SetType(EProjectionType(reader.Read_byte()));

	if (_projection.GetType() == EProjectionType::PERSPECTIVE)
		_projection.SetPerspectiveFOV(reader.Read_float());
	else
		_projection.SetOrthographicScale(reader.Read_float());

	float n = reader.Read_float();
	float f = reader.Read_float();
	_projection.SetNearFar(n, f);

	uint16 x = reader.Read_uint16();
	uint16 y = reader.Read_uint16();
	_projection.SetDimensions(Vector2T<uint16>(x, y));
}
