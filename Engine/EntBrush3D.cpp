#include "EntBrush3D.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/MacroUtilities.hpp>
#include <ELCore/Utilities.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

void EntBrush3D::_OnTransformChanged()
{
	if (!_updatingTransform)
	{
		float hw = GetRelativeScale().x / 2.f;
		float hh = GetRelativeScale().y / 2.f;
		float hd = GetRelativeScale().z / 2.f;

		_point1 = Vector3(GetRelativePosition().x - hw, GetRelativePosition().y - hh, GetRelativePosition().z - hd);
		_point2 = Vector3(GetRelativePosition().x + hw, GetRelativePosition().y + hh, GetRelativePosition().z + hd);
	}
}

void EntBrush3D::_OnPointChanged()
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

void EntBrush3D::Render(RenderQueue& q) const
{
	if (_material)
	{
		RenderEntry& e = q.CreateEntry(ERenderChannels::SURFACE, 0);
		_material->Apply(e);
		e.AddCommand(RCMDSetUVOffset::Default());
		e.AddSetColour(Colour::White);
		
		Matrix4 pt = _parent ? _parent->GetTransformationMatrix() : Matrix4();
		Transform t;
		
		//Front
		t.SetScale(Vector3(GetRelativeScale().x, GetRelativeScale().y, 1.f));
		t.SetPosition(GetRelativePosition() + Vector3(0.f, 0.f, -GetRelativeScale().z / 2.f));
		e.AddSetUVScale(Vector2(GetRelativeScale().x, GetRelativeScale().y));
		e.AddSetTransform(t.MakeTransformationMatrix() * pt);
		e.AddCommand(RCMDRenderMesh::PLANE);

		//Back
		t.SetRotation(Vector3(0.f, 180.f, 0.f));
		t.Move(Vector3(0.f, 0.f, GetRelativeScale().z));
		e.AddSetTransform(t.MakeTransformationMatrix() * pt);
		e.AddCommand(RCMDRenderMesh::PLANE);

		//Left
		t.SetScale(Vector3(GetRelativeScale().z, GetRelativeScale().y, 1.f));
		t.SetRotation(Vector3(0.f, 90.f, 0.f));
		t.SetPosition(Vector3(GetRelativePosition() - Vector3(GetRelativeScale().x / 2.f, 0.f, 0.f)));
		e.AddSetUVScale(Vector2(GetRelativeScale().z, GetRelativeScale().y));
		e.AddSetTransform(t.MakeTransformationMatrix() * pt);
		e.AddCommand(RCMDRenderMesh::PLANE);

		//Right
		t.SetRotation(Vector3(0.f, -90.f, 0.f));
		t.Move(Vector3(GetRelativeScale().x, 0.f, 0.f));
		e.AddSetTransform(t.MakeTransformationMatrix() * pt);
		e.AddCommand(RCMDRenderMesh::PLANE);

		//Bottom
		t.SetScale(Vector3(GetRelativeScale().x, GetRelativeScale().z, 1.f));
		t.SetRotation(Vector3(90.f, 0.f, 0.f));
		t.SetPosition(GetRelativePosition() - Vector3(0.f, GetRelativeScale().y / 2.f, 0.f));
		e.AddSetUVScale(Vector2(GetRelativeScale().x, GetRelativeScale().z));
		e.AddSetTransform(t.MakeTransformationMatrix() * pt);
		e.AddCommand(RCMDRenderMesh::PLANE);

		//Top
		t.SetRotation(Vector3(-90.f, 0.f, 0.f));
		t.Move(Vector3(0.f, GetRelativeScale().y, 0.f));
		e.AddSetTransform(t.MakeTransformationMatrix() * pt);
		e.AddCommand(RCMDRenderMesh::PLANE);
	}
}

void EntBrush3D::WriteData(ByteWriter &writer, NumberedSet<String> &strings, const Context& ctx) const
{
	Entity::WriteData(writer, strings, ctx);

	MaterialManager* materialManager = ctx.GetPtr<MaterialManager>();
	if (materialManager && _material)
	{
		uint16 id = strings.Add(materialManager->FindNameOf(_material.Ptr()));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);
}

void EntBrush3D::ReadData(ByteReader &reader, const NumberedSet<String> &strings, const Context& ctx)
{
	Entity::ReadData(reader, strings, ctx);

	const String *materialName = strings.Get(reader.Read_uint16());
	if (materialName)
		SetMaterial(*materialName, ctx);

	_point1.x = GetRelativePosition().x - GetRelativeScale().x / 2.f;
	_point1.y = GetRelativePosition().y - GetRelativeScale().y / 2.f;
	_point1.z = GetRelativePosition().z - GetRelativeScale().z / 2.f;
	_point2.x = GetRelativePosition().x + GetRelativeScale().x / 2.f;
	_point2.y = GetRelativePosition().y + GetRelativeScale().y / 2.f;
	_point2.z = GetRelativePosition().z + GetRelativeScale().z / 2.f;
}

const PropertyCollection& EntBrush3D::GetProperties()
{
	static PropertyCollection cvars;

	DO_ONCE_BEGIN;
	_AddBaseProperties(cvars);

	cvars.Add(
		"Material", 
		ContextualMemberGetter<EntBrush3D, String>(&EntBrush3D::GetMaterialName), 
		ContextualMemberSetter<EntBrush3D, String>(&EntBrush3D::SetMaterial), 
		0,
		PropertyFlags::MATERIAL);
		
	cvars.Add(
		"Point1", 
		MemberGetter<EntBrush3D, const Vector3&>(&EntBrush3D::GetPoint1),
		MemberSetter<EntBrush3D, Vector3>(&EntBrush3D::SetPoint1));

	cvars.Add(
		"Point2",
		MemberGetter<EntBrush3D, const Vector3&>(&EntBrush3D::GetPoint2),
		MemberSetter<EntBrush3D, Vector3>(&EntBrush3D::SetPoint2));

	DO_ONCE_END;

	return cvars;
}
