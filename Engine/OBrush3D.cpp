#include "OBrush3D.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/MacroUtilities.hpp>
#include <ELCore/Utilities.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELPhys/CollisionBox.hpp>

void OBrush3D::_InitBody(FixedBody& body) const
{
	body.Collision() = Collider(ECollisionChannels::SURFACE, CollisionBox(Box(Vector3(), Vector3(.5f, .5f, .5f))));
}

void OBrush3D::_UpdateBody(FixedBody& body) const
{
	body.SetTransform(GetAbsoluteTransform());
}

void OBrush3D::_OnTransformChanged()
{
	ORenderable::_OnTransformChanged();

	if (!_updatingTransform)
	{
		float hw = GetRelativeScale().x / 2.f;
		float hh = GetRelativeScale().y / 2.f;
		float hd = GetRelativeScale().z / 2.f;

		_point1 = Vector3(GetRelativePosition().x - hw, GetRelativePosition().y - hh, GetRelativePosition().z - hd);
		_point2 = Vector3(GetRelativePosition().x + hw, GetRelativePosition().y + hh, GetRelativePosition().z + hd);
	}
}

void OBrush3D::_OnPointChanged()
{
	float x = (_point1.x + _point2.x) / 2.f;
	float y = (_point1.y + _point2.y) / 2.f;
	float z = (_point1.z + _point2.z) / 2.f;
	float w = Maths::Abs(_point1.x - _point2.x);
	float h = Maths::Abs(_point1.y - _point2.y);
	float d = Maths::Abs(_point1.z - _point2.z);

	_updatingTransform = true;
	SetRelativePosition(Vector3(x, y, z));
	SetRelativeScale(Vector3(w, h, d));
	_updatingTransform = false;
}

void OBrush3D::Render(RenderQueue& q) const
{
	if (_material)
	{
		RenderEntry& e = q.CreateEntry(ERenderChannels::SURFACE, 0);
		_material->Apply(e);
		e.AddCommand(RCMDSetUVOffset::Default());
		e.AddSetColour(Colour::White);
		
		Matrix4 pt = GetParent() ? GetParent()->GetAbsoluteTransform().GetMatrix() : Matrix4();
		Transform t;
		
		//Front
		t.SetScale(Vector3(GetRelativeScale().x, GetRelativeScale().y, 1.f));
		t.SetPosition(GetRelativePosition() + Vector3(0.f, 0.f, -GetRelativeScale().z / 2.f));
		e.AddSetUVScale(Vector2(GetRelativeScale().x, GetRelativeScale().y));
		e.AddSetTransform(t.MakeMatrix() * pt);
		e.AddCommand(RCMDRenderMesh::PLANE);

		//Back
		t.SetRotation(Vector3(0.f, 180.f, 0.f));
		t.Move(Vector3(0.f, 0.f, GetRelativeScale().z));
		e.AddSetTransform(t.MakeMatrix() * pt);
		e.AddCommand(RCMDRenderMesh::PLANE);

		//Left
		t.SetScale(Vector3(GetRelativeScale().z, GetRelativeScale().y, 1.f));
		t.SetRotation(Vector3(0.f, 90.f, 0.f));
		t.SetPosition(Vector3(GetRelativePosition() - Vector3(GetRelativeScale().x / 2.f, 0.f, 0.f)));
		e.AddSetUVScale(Vector2(GetRelativeScale().z, GetRelativeScale().y));
		e.AddSetTransform(t.MakeMatrix() * pt);
		e.AddCommand(RCMDRenderMesh::PLANE);

		//Right
		t.SetRotation(Vector3(0.f, -90.f, 0.f));
		t.Move(Vector3(GetRelativeScale().x, 0.f, 0.f));
		e.AddSetTransform(t.MakeMatrix() * pt);
		e.AddCommand(RCMDRenderMesh::PLANE);

		//Bottom
		t.SetScale(Vector3(GetRelativeScale().x, GetRelativeScale().z, 1.f));
		t.SetRotation(Vector3(90.f, 0.f, 0.f));
		t.SetPosition(GetRelativePosition() - Vector3(0.f, GetRelativeScale().y / 2.f, 0.f));
		e.AddSetUVScale(Vector2(GetRelativeScale().x, GetRelativeScale().z));
		e.AddSetTransform(t.MakeMatrix() * pt);
		e.AddCommand(RCMDRenderMesh::PLANE);

		//Top
		t.SetRotation(Vector3(-90.f, 0.f, 0.f));
		t.Move(Vector3(0.f, GetRelativeScale().y, 0.f));
		e.AddSetTransform(t.MakeMatrix() * pt);
		e.AddCommand(RCMDRenderMesh::PLANE);
	}
}

void OBrush3D::Read(ByteReader& reader, ObjectIOContext& ctx)
{
	WorldObject::Read(reader, ctx);

	const String* materialName = ctx.strings.TryGet(reader.Read_uint16());
	if (materialName)
		SetMaterial(*materialName, ctx.context);

	_point1.x = GetRelativePosition().x - GetRelativeScale().x / 2.f;
	_point1.y = GetRelativePosition().y - GetRelativeScale().y / 2.f;
	_point1.z = GetRelativePosition().z - GetRelativeScale().z / 2.f;
	_point2.x = GetRelativePosition().x + GetRelativeScale().x / 2.f;
	_point2.y = GetRelativePosition().y + GetRelativeScale().y / 2.f;
	_point2.z = GetRelativePosition().z + GetRelativeScale().z / 2.f;
}

void OBrush3D::Write(ByteWriter &writer, ObjectIOContext& ctx) const
{
	WorldObject::Write(writer, ctx);

	MaterialManager* materialManager = ctx.context.GetPtr<MaterialManager>();
	if (materialManager && _material)
	{
		uint16 id = ctx.strings.Add(materialManager->FindNameOf(_material.Ptr()));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);
}

const PropertyCollection& OBrush3D::GetProperties()
{
	static PropertyCollection properties(WorldObject::_GetNonTransformProperties());

	DO_ONCE_BEGIN;
	properties.Add(
		"Material", 
		ContextualMemberGetter<OBrush3D, String>(&OBrush3D::GetMaterialName), 
		ContextualMemberSetter<OBrush3D, String>(&OBrush3D::SetMaterial), 
		0,
		PropertyFlags::MATERIAL);
		
	properties.Add(
		"Point1", 
		MemberGetter<OBrush3D, const Vector3&>(&OBrush3D::GetPoint1),
		MemberSetter<OBrush3D, Vector3>(&OBrush3D::SetPoint1));

	properties.Add(
		"Point2",
		MemberGetter<OBrush3D, const Vector3&>(&OBrush3D::GetPoint2),
		MemberSetter<OBrush3D, Vector3>(&OBrush3D::SetPoint2));

	DO_ONCE_END;

	return properties;
}
