#include "OBrush2D.hpp"
#include "World.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/NumberedSet.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELPhys/CollisionBox.hpp>

OBrush2D::OBrush2D(World& world) :
	ORenderable(world),
	_axis(EAxis::Y),
	_updatingTransform(false),
	_level(0.f),
	_volume(Transform(Vector3(), Rotation(), Vector3(1.f, 0.f, 1.f)))
{}

OBrush2D::OBrush2D(const OBrush2D& other) :
	ORenderable(other),
	_axis(other._axis),
	_point1(other._point1),
	_point2(other._point2),
	_level(other._level),
	_volume(other._volume),
	_updatingTransform(false)
{}

void OBrush2D::_InitBody(FixedBody& body) const
{
	body.Collision() = Collider(ECollisionChannels::SURFACE, CollisionBox(Box(Vector3(0.f, -.5f, 0.f), Vector3(.5f, .5f, .5f))));
}

void OBrush2D::_UpdateBody(FixedBody& body) const
{
	body.SetTransform(GetAbsoluteTransform());
}

void OBrush2D::_OnTransformChanged()
{
	ORenderable::_OnTransformChanged();

	if (!_updatingTransform)
	{
		float hw = GetRelativeScale().x / 2.f;
		float hh = GetRelativeScale().z / 2.f;

		_point1 = Vector2(GetRelativePosition().x - hw, GetRelativePosition().z - hh);
		_point2 = Vector2(GetRelativePosition().x + hw, GetRelativePosition().z + hh);
	}
}

void OBrush2D::_OnPointChanged()
{
	float x = (_point1.x + _point2.x) / 2.f;
	float z = (_point1.y + _point2.y) / 2.f;
	float w = Maths::Abs(_point1.x - _point2.x);
	float h = Maths::Abs(_point1.y - _point2.y);

	_updatingTransform = true;
	SetRelativePosition(Vector3(x, _level, z));
	SetRelativeScale(Vector3(w, 1.f, h));
	_updatingTransform = false;
}

void OBrush2D::Render(RenderQueue& q) const
{
	if (_material)
	{
		RenderEntry& e = q.CreateEntry(ERenderChannels::SURFACE, 0);
		_material->Apply(e);
		e.AddSetUVScale(Vector2(GetRelativeScale().x, GetRelativeScale().z));
		e.AddCommand(RCMDSetUVOffset::Default());
		e.AddSetColour(Colour::White);
		e.AddSetTransform(Matrix4::RotationX(-90.f) * GetAbsoluteTransform().GetMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);
		e.AddSetTransform(Matrix4::RotationX(90.f) * GetAbsoluteTransform().GetMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);
	}
}

void OBrush2D::Read(ByteReader &reader, ObjectIOContext& ctx)
{
	WorldObject::Read(reader, ctx);

	const String *materialName = ctx.strings.TryGet(reader.Read_uint16());
	if (materialName)
		SetMaterial(*materialName, ctx.context);

	_level = GetRelativePosition().y;
	_point1.x = GetRelativePosition().x - GetRelativeScale().x / 2.f;
	_point1.y = GetRelativePosition().z - GetRelativeScale().z / 2.f;
	_point2.x = GetRelativePosition().x + GetRelativeScale().x / 2.f;
	_point2.y = GetRelativePosition().z + GetRelativeScale().z / 2.f;
}

void OBrush2D::Write(ByteWriter& writer, ObjectIOContext& ctx) const
{
	WorldObject::Write(writer, ctx);

	MaterialManager* materialManager = ctx.context.GetPtr<MaterialManager>();
	if (_material)
	{
		uint16 id = ctx.strings.Add(materialManager->FindNameOf(_material.Ptr()));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);
}


const PropertyCollection& OBrush2D::GetProperties()
{
	static PropertyCollection properties(WorldObject::_GetNonTransformProperties());
	
	DO_ONCE_BEGIN;
	properties.Add(
		"Material",
		ContextualMemberGetter<OBrush2D, String>(&OBrush2D::GetMaterialName),
		ContextualMemberSetter<OBrush2D, String>(&OBrush2D::SetMaterial),
		0,
		PropertyFlags::MATERIAL);

	properties.Add(
		"Point1",
		MemberGetter<OBrush2D, const Vector2&>(&OBrush2D::GetPoint1),
		MemberSetter<OBrush2D, Vector2>(&OBrush2D::SetPoint1));

	properties.Add(
		"Point2",
		MemberGetter<OBrush2D, const Vector2&>(&OBrush2D::GetPoint2),
		MemberSetter<OBrush2D, Vector2>(&OBrush2D::SetPoint2));

	properties.Add(
		"Level",
		MemberGetter<OBrush2D, float>(&OBrush2D::GetLevel),
		MemberSetter<OBrush2D, float>(&OBrush2D::SetLevel));
	DO_ONCE_END;

	return properties;
}
