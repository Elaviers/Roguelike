#include "OLight.hpp"
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

Vector3 OLight::_editorBoxExtent(.1f, .1f, .1f);

bool OLight::drawLightSources = true;

const PropertyCollection& OLight::GetProperties()
{
	static PropertyCollection properties(WorldObject::GetProperties());

	DO_ONCE_BEGIN;
	properties.Add<Vector3>(
		"Colour", 
		offsetof(OLight, _colour));

	properties.Add(
		"Radius", 
		MemberGetter<OLight, float>(&OLight::GetRadius),
		MemberSetter<OLight, float>(&OLight::SetRadius));
	DO_ONCE_END;

	return properties;
}

void OLight::SetRadius(const float& radius)
{
	_radius = radius; 
}

void OLight::Render(RenderQueue& q) const
{
	RenderEntry& e = q.CreateEntry(ERenderChannels::SURFACE, -1);
	e.AddLight(GetAbsolutePosition(), _colour, _radius);

	if (OLight::drawLightSources)
	{
		const float colour[4] = { _colour.x, _colour.y, _colour.z, 1.f };
		RenderEntry& de = q.CreateEntry(ERenderChannels::EDITOR);
		de.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
		de.AddSetColour(Colour(Vector4(_colour, 1.f)));
		de.AddSetTransform(Matrix4::Scale(_editorBoxExtent * 2.f) * GetAbsoluteTransform().GetMatrix());
		de.AddCommand(RCMDRenderMesh::CUBE);
	}
}

bool OLight::IsVisible(const Frustum& view) const
{
	if (_radius < 0.f) return true;

	return view.OverlapsSphere(GetAbsolutePosition(), _radius);
}

void OLight::Read(ByteReader& reader, ObjectIOContext& ctx)
{
	WorldObject::Read(reader, ctx);

	_radius = reader.Read_float();
	_colour.Read(reader);
}

void OLight::Write(ByteWriter& writer, ObjectIOContext& ctx) const
{
	WorldObject::Write(writer, ctx);

	writer.Write_float(_radius);
	_colour.Write(writer);
}

