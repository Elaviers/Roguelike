#include "EntLight.hpp"
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

Vector3 EntLight::_editorBoxExtent(.1f, .1f, .1f);

bool EntLight::drawLightSources = true;

const PropertyCollection& EntLight::GetProperties()
{
	static PropertyCollection cvars;

	DO_ONCE_BEGIN;
	_AddBaseProperties(cvars);

	cvars.Add<Vector4>(
		"Colour", 
		offsetof(EntLight, _colour));

	cvars.Add(
		"Radius", 
		MemberGetter<EntLight, float>(&EntLight::GetRadius),
		MemberSetter<EntLight, float>(&EntLight::SetRadius));
	DO_ONCE_END;

	return cvars;
}

void EntLight::SetRadius(const float& radius)
{
	_radius = radius; 
	_bounds = Bounds(Maths::Min(_radius, 0.f));
	_InvalidateWorldBounds();

	if (_radius < 0.f)
		_flags |= EFlags::ALWAYS_DRAW;
	else
		_flags &= ~EFlags::ALWAYS_DRAW;
}

void EntLight::Render(RenderQueue& q) const
{
	RenderEntry& e = q.CreateEntry(ERenderChannels::SURFACE, -1);
	e.AddLight(GetWorldTransform().GetPosition(), _colour, _radius);

	if (EntLight::drawLightSources)
	{
		const float colour[4] = { _colour.x, _colour.y, _colour.z, 1.f };
		RenderEntry& de = q.CreateEntry(ERenderChannels::UNLIT);
		de.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
		de.AddSetColour(Colour(Vector4(_colour, 1.f)));
		de.AddSetTransform(Matrix4::Scale(_editorBoxExtent * 2.f) * GetTransformationMatrix());
		de.AddCommand(RCMDRenderMesh::CUBE);
	}
}

void EntLight::WriteData(ByteWriter& writer, NumberedSet<String>& strings, const Context& ctx) const
{
	Entity::WriteData(writer, strings, ctx);

	writer.Write_float(_radius);
	_colour.Write(writer);
}

void EntLight::ReadData(ByteReader& reader, const NumberedSet<String>& strings, const Context& ctx)
{
	Entity::ReadData(reader, strings, ctx);

	_radius = reader.Read_float();
	_colour.Read(reader);
}
