#include "EntSprite.hpp"
#include "EntCamera.hpp"
#include <ELCore/Context.hpp>
#include <ELGraphics/MaterialManager.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

void EntSprite::Render(RenderQueue& q) const
{
	if (_material)
	{
		RenderEntry& e = q.CreateEntry(ERenderChannels::SPRITE);

		Matrix4 t;

		if (_fixedYaw)
		{
			Vector3 pos = GetWorldPosition();
			Vector3 delta = pos - EntCamera::Current()->GetWorldPosition();
			delta.Normalise();

			float y = Maths::ArcTangentDegrees2(delta.x, delta.z);
			Vector3 e = Vector3(0, y, 0);
			t = Matrix4::Transformation(pos, e, Vector3(_size, _size, _size));
		}
		else
			t = Matrix4::Transformation(GetWorldPosition(), EntCamera::Current()->GetWorldRotation().GetQuat(), Vector3(_size, _size, _size));
		
		_material->Apply(e);
		e.AddCommand(RCMDSetUVOffset::Default());
		e.AddCommand(RCMDSetUVScale::Default());
		e.AddSetTransform(t);
		e.AddSetColour(_colour);
		e.AddCommand(RCMDRenderMesh::PLANE);
	}
}

const PropertyCollection& EntSprite::GetProperties()
{
	static PropertyCollection cvars;

	DO_ONCE_BEGIN;
	_AddBaseProperties(cvars);

	cvars.Add(
		"Material",
		ContextualMemberGetter<EntSprite, String>(&EntSprite::GetMaterialName),
		ContextualMemberSetter<EntSprite, String>(&EntSprite::SetMaterialName),
		0,
		PropertyFlags::MATERIAL);

	cvars.Add<float>(
		"Size",
		offsetof(EntSprite, _size));

	cvars.Add<bool>(
		"FixedYaw",
		offsetof(EntSprite, _fixedYaw));
	DO_ONCE_END;

	return cvars;
}

void EntSprite::WriteData(ByteWriter& writer, NumberedSet<String>& strings, const Context& ctx) const
{
	Entity::WriteData(writer, strings, ctx);

	writer.Write_float(_size);
	((const Vector4&)_colour).Write(writer);

	MaterialManager* materialManager = ctx.GetPtr<MaterialManager>();
	if (_material)
	{
		uint16 id = strings.Add(materialManager->FindNameOf(_material.Ptr()));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);
}

void EntSprite::ReadData(ByteReader& reader, const NumberedSet<String>& strings, const Context& ctx)
{
	Entity::ReadData(reader, strings, ctx);

	_size = reader.Read_float();
	((Vector4&)_colour).Read(reader);
	
	const String* materialName = strings.Get(reader.Read_uint16());

	MaterialManager* materialManager = ctx.GetPtr<MaterialManager>();
	if (materialName)
		_material = materialManager->Get(*materialName, ctx).Cast<const MaterialSprite>();
}
