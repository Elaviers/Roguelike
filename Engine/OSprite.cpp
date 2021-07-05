#include "OSprite.hpp"
#include "OCamera.hpp"
#include <ELCore/Context.hpp>
#include <ELGraphics/MaterialManager.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

void OSprite::Render(RenderQueue& q) const
{
	if (_material)
	{
		RenderEntry& e = q.CreateEntry(ERenderChannels::SPRITE);
		_material->Apply(e);
		e.AddCommand(RCMDSetUVOffset::Default());
		e.AddCommand(RCMDSetUVScale::Default());
		e.AddSetMat4(RCMDSetMat4::Type::TRANSFORM_CAMERAFACING_Y, Matrix4::Transformation(GetAbsolutePosition(), Quaternion(), _size * GetAbsoluteScale()));
		e.AddSetColour(_colour);
		e.AddCommand(RCMDRenderMesh::PLANE);
	}
}

const PropertyCollection& OSprite::GetProperties()
{
	static PropertyCollection properties(WorldObject::GetProperties());

	DO_ONCE_BEGIN;
	properties.Add(
		"Material",
		ContextualMemberGetter<OSprite, String>(&OSprite::GetMaterialName),
		ContextualMemberSetter<OSprite, String>(&OSprite::SetMaterialName),
		0,
		PropertyFlags::MATERIAL);

	properties.Add<float>(
		"Size",
		offsetof(OSprite, _size));

	properties.Add<bool>(
		"FixedYaw",
		offsetof(OSprite, _fixedYaw));
	DO_ONCE_END;

	return properties;
}

bool OSprite::IsVisible(const Frustum& view) const
{
	return view.OverlapsSphere(GetAbsolutePosition(), _size * Maths::Max(GetAbsoluteScale().GetData(), 2)); //todo- don't use max, I need the hypotenuse here to represent square sprites....
}

void OSprite::Read(ByteReader& reader, ObjectIOContext& ctx)
{
	WorldObject::Read(reader, ctx);

	_size = reader.Read_float();
	((Vector4&)_colour).Read(reader);
	
	const String* materialName = ctx.strings.TryGet(reader.Read_uint16());

	MaterialManager* materialManager = ctx.context.GetPtr<MaterialManager>();
	if (materialName)
		_material = materialManager->Get(*materialName, ctx.context).Cast<const MaterialSprite>();
}

void OSprite::Write(ByteWriter& writer, ObjectIOContext& ctx) const
{
	WorldObject::Write(writer, ctx);

	writer.Write_float(_size);
	((const Vector4&)_colour).Write(writer);

	MaterialManager* materialManager = ctx.context.GetPtr<MaterialManager>();
	if (_material)
	{
		uint16 id = ctx.strings.Add(materialManager->FindNameOf(_material.Ptr()));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);
}
