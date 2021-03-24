#include "OCamera.hpp"
#include <ELGraphics/RenderQueue.hpp>
#include <ELSys/GLProgram.hpp>

void OCamera::Use(RenderQueue& queue, ERenderChannels channels) const
{
	queue.CreateCameraEntry(_projection, GetAbsoluteTransform(), channels);
}

void OCamera::Use(RenderQueue& queue, int vpX, int vpY, ERenderChannels channels) const
{
	RenderEntry& e = queue.CreateEntry(channels);
	e.AddSetViewport(vpX, vpY, _projection.GetDimensions().x, _projection.GetDimensions().y);
	e.AddSetMat4(RCMDSetMat4::Type::VIEW, GetAbsoluteTransform().GetInverseMatrix());
	e.AddSetMat4(RCMDSetMat4::Type::PROJECTION, _projection.GetMatrix());
}

const PropertyCollection& OCamera::GetProperties()
{
	static PropertyCollection properties(WorldObject::GetProperties());
	DO_ONCE_BEGIN;
	
	//todo- camera properties

	DO_ONCE_END;

	return properties;
}

void OCamera::Write(ByteWriter& writer, ObjectIOContext& ctx) const
{
	WorldObject::Write(writer, ctx);

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

void OCamera::Read(ByteReader& reader, ObjectIOContext& ctx)
{
	WorldObject::Read(reader, ctx);

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
