#include "EntBrush2D.hpp"
#include <ELCore/Context.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

void EntBrush2D::_OnTransformChanged()
{
	if (!_updatingTransform)
	{
		float hw = GetRelativeScale().x / 2.f;
		float hh = GetRelativeScale().y / 2.f;

		_point1 = Vector2(GetRelativePosition().x - hw, GetRelativePosition().z - hh);
		_point2 = Vector2(GetRelativePosition().x + hh, GetRelativePosition().z + hh);
	}
}

void EntBrush2D::_OnPointChanged()
{
	float x = (_point1.x + _point2.x) / 2.f;
	float z = (_point1.y + _point2.y) / 2.f;
	float w = Maths::Abs(_point1.x - _point2.x);
	float h = Maths::Abs(_point1.y - _point2.y);

	_updatingTransform = true;
	SetRelativePosition(Vector3(x, _level, z));
	SetRelativeScale(Vector3(w, 0.f, h));
	_updatingTransform = false;
}

void EntBrush2D::Render(RenderQueue& q) const
{
	if (_material)
	{
		RenderEntry& e = q.NewDynamicEntry(ERenderChannels::SURFACE, 0);
		_material->Apply(e);
		e.AddSetUVScale(Vector2(GetRelativeScale().x, GetRelativeScale().z));
		e.AddCommand(RCMDSetUVOffset::Default());
		e.AddSetColour(Colour::White);
		e.AddSetTransform(Matrix4::RotationX(-90.f) * GetTransformationMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);
		e.AddSetTransform(Matrix4::RotationX(90.f) * GetTransformationMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);
	}
}


void EntBrush2D::WriteData(ByteWriter& writer, NumberedSet<String>& strings, const Context& ctx) const
{
	Entity::WriteData(writer, strings, ctx);

	MaterialManager* materialManager = ctx.GetPtr<MaterialManager>();
	if (_material)
	{
		uint16 id = strings.Add(materialManager->FindNameOf(_material.Ptr()));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);
}

void EntBrush2D::ReadData(ByteReader &reader, const NumberedSet<String> &strings, const Context& ctx)
{
	Entity::ReadData(reader, strings, ctx);

	const String *materialName = strings.Get(reader.Read_uint16());
	if (materialName)
		SetMaterial(*materialName, ctx);

	_level = GetRelativePosition().y;
	_point1.x = GetRelativePosition().x - GetRelativeScale().x / 2.f;
	_point1.y = GetRelativePosition().z - GetRelativeScale().y / 2.f;
	_point2.x = GetRelativePosition().x + GetRelativeScale().x / 2.f;
	_point2.y = GetRelativePosition().z + GetRelativeScale().y / 2.f;
}

const PropertyCollection& EntBrush2D::GetProperties()
{
	static PropertyCollection cvars;
	
	DO_ONCE_BEGIN;
	cvars.Add(
		"Material",
		ContextualMemberGetter<EntBrush2D, String>(&EntBrush2D::GetMaterialName),
		ContextualMemberSetter<EntBrush2D, String>(&EntBrush2D::SetMaterial),
		0,
		PropertyFlags::MATERIAL);

	cvars.Add(
		"Point1",
		MemberGetter<EntBrush2D, const Vector2&>(&EntBrush2D::GetPoint1),
		MemberSetter<EntBrush2D, Vector2>(&EntBrush2D::SetPoint1));

	cvars.Add(
		"Point2",
		MemberGetter<EntBrush2D, const Vector2&>(&EntBrush2D::GetPoint2),
		MemberSetter<EntBrush2D, Vector2>(&EntBrush2D::SetPoint2));

	cvars.Add(
		"Level",
		MemberGetter<EntBrush2D, float>(&EntBrush2D::GetLevel),
		MemberSetter<EntBrush2D, float>(&EntBrush2D::SetLevel));
	DO_ONCE_END;

	return cvars;
}
