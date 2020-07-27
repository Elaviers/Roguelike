#include "EntRenderable.hpp"
#include <ELCore/Context.hpp>
#include <ELGraphics/MeshManager.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

const PropertyCollection& EntRenderable::GetProperties()
{
	static PropertyCollection cvars;

	DO_ONCE_BEGIN;
	_AddBaseProperties(cvars);
	cvars.Add(
		"Model", 
		ContextualMemberGetter<EntRenderable, String>(&EntRenderable::GetModelName), 
		ContextualMemberSetter<EntRenderable, String>(&EntRenderable::SetModel), 
		0,
		PropertyFlags::MODEL);

	cvars.Add(
		"Material", 
		ContextualMemberGetter<EntRenderable, String>(&EntRenderable::GetMaterialName), 
		ContextualMemberSetter<EntRenderable, String>(&EntRenderable::SetMaterial), 
		0,
		PropertyFlags::MATERIAL);
	DO_ONCE_END;

	return cvars;
}

void EntRenderable::Render(RenderQueue& q) const
{
	if (_model)
	{
		RenderEntry& e = q.NewDynamicEntry(ERenderChannels::SURFACE);

		if (_material)
		{
			_material->Apply(e);
		}

		e.AddCommand(RCMDSetUVOffset::Default());
		e.AddCommand(RCMDSetUVScale::Default());

		e.AddSetColour(_colour);
		e.AddSetTransform(GetTransformationMatrix());
		e.AddRenderMesh(*_model->GetMesh());
	}
}

const Collider* EntRenderable::GetCollider() const
{
	if (_model)
		return &_model->GetCollider();

	return nullptr;
}

String EntRenderable::GetModelName(const Context& ctx) const
{
	if (_model) return ctx.GetPtr<ModelManager>()->FindNameOf(_model.Ptr());
	return "Unknown";
}

String EntRenderable::GetMaterialName(const Context& ctx) const
{																	//todo: remove Smelly const cast
	if (_material) return ctx.GetPtr<MaterialManager>()->FindNameOf(const_cast<Material*>(_material.Ptr()));
	return "Unknown";
}

void EntRenderable::WriteData(ByteWriter &writer, NumberedSet<String> &strings, const Context& ctx) const
{
	Entity::WriteData(writer, strings, ctx);

	ModelManager* modelManager = ctx.GetPtr<ModelManager>();
	if (_model)
	{
		uint16 id = strings.Add(modelManager->FindNameOf(_model.Ptr()));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);

	MaterialManager* materialManager = ctx.GetPtr<MaterialManager>();
	if (!_materialIsDefault && _material)
	{																	//todo: const cast removal
		uint16 id = strings.Add(materialManager->FindNameOf(const_cast<Material*>(_material.Ptr())));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);

	((const Vector4&)_colour).Write(writer);
}

void EntRenderable::ReadData(ByteReader &reader, const NumberedSet<String> &strings, const Context& ctx)
{
	Entity::ReadData(reader, strings, ctx);

	const String *string;

	if (string = strings.Get(reader.Read_uint16()))
		SetModel(*string, ctx);

	if (string = strings.Get(reader.Read_uint16()))
		SetMaterial(*string, ctx);

	((Vector4&)_colour).Read(reader);
}
