#include "ORenderable.hpp"
#include "World.hpp"
#include <ELCore/Context.hpp>
#include <ELGraphics/MaterialParam.hpp>
#include <ELGraphics/MeshManager.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

ORenderable::ORenderable(World& world) :
	WorldObject(world),
	_renderEntry(ERenderChannels::SURFACE),
	_colour(1.f, 1.f, 1.f),
	_materialIsDefault(false),
	_static(false),
	_physics(world.CreateFixedBody(this))
{}

ORenderable::ORenderable(const ORenderable& other) :
	WorldObject(other),
	_renderEntry(ERenderChannels::SURFACE),
	_model(other._model),
	_material(other._material),
	_colour(other._colour),
	_materialIsDefault(other._materialIsDefault),
	_static(other._static),
	_physics(GetWorld().CreateFixedBody(this, *other._physics))
{}

void ORenderable::_OnCreated()
{
	WorldObject::_OnCreated();
	_P_InitFixedBody();
}

void ORenderable::_OnTransformChanged()
{
	WorldObject::_OnTransformChanged();
	_P_UpdateFixedBody();
	_UpdateRenderEntry();
}

void ORenderable::_UpdateBody(FixedBody& body) const
{
	if (_model)
	{
		body.SetTransform(GetAbsoluteTransform());
		body.Collision() = _model->GetCollider();
	}
	else body.Collision().ClearShapes();
}

void ORenderable::_UpdateRenderEntry()
{
	if (!_static)
		return;

	_renderEntry.Clear();

	if (_model)
	{
		_renderEntry.AddCommand(RCMDSetUVOffset::Default());
		_renderEntry.AddCommand(RCMDSetUVScale::Default());

		_renderEntry.AddSetColour(_colour);
		_renderEntry.AddSetTransform(GetAbsoluteTransform().GetMatrix());

		if (_material)
			_material->Apply(_renderEntry);

		_renderEntry.AddRenderMesh(*_model->GetMesh());
	}
}

const PropertyCollection& ORenderable::GetProperties()
{
	static PropertyCollection properties(WorldObject::GetProperties());

	DO_ONCE_BEGIN;
	properties.Add(
		"Model", 
		ContextualMemberGetter<ORenderable, String>(&ORenderable::GetModelName), 
		ContextualMemberSetter<ORenderable, String>(&ORenderable::SetModel), 
		0,
		PropertyFlags::MODEL);

	properties.Add(
		"Material", 
		ContextualMemberGetter<ORenderable, String>(&ORenderable::GetMaterialName), 
		ContextualMemberSetter<ORenderable, String>(&ORenderable::SetMaterial), 
		0,
		PropertyFlags::MATERIAL);
	DO_ONCE_END;

	return properties;
}

void ORenderable::Render(RenderQueue& q) const
{
	if (_static)
		q.AddEntry(&_renderEntry);
	else
	{
		if (_model)
		{
			RenderEntry& e = q.CreateEntry(ERenderChannels::SURFACE);
			e.AddCommand(RCMDSetUVOffset::Default());
			e.AddCommand(RCMDSetUVScale::Default());

			e.AddSetColour(_colour);
			e.AddSetTransform(GetAbsoluteTransform().GetMatrix());

			if (_material)
			{
				MaterialParam param;
				param.type = MaterialParam::EType::SPRITESHEET_PARAM;
				param.spritesheetData.time = _age;

				_material->Apply(e, &param);
			}

			e.AddRenderMesh(*_model->GetMesh());
		}
	}
}

const Volume* ORenderable::GetVolume() const
{
	return _model ? _model->GetMesh()->GetVolume() : nullptr;
}

String ORenderable::GetModelName(const Context& ctx) const
{
	if (_model) return ctx.GetPtr<ModelManager>()->FindNameOf(_model.Ptr());
	return "Unknown";
}

String ORenderable::GetMaterialName(const Context& ctx) const
{
	if (_material) return ctx.GetPtr<MaterialManager>()->FindNameOf(_material.Ptr());
	return "Unknown";
}

void ORenderable::SetMaterial(const SharedPointer<const Material>& material) 
{ 
	_material = material; 
	_materialIsDefault = false;
	_UpdateRenderEntry();
}

void ORenderable::SetModel(const SharedPointer<const Model>& model, const Context& ctx)
{
	_model = model;

	if (_model && _model->GetDefaultMaterialName().GetLength() != 0)
	{
		SetMaterial(model->GetDefaultMaterialName(), ctx);
		_materialIsDefault = true;
	}
	else SetMaterial(SharedPointer<const Material>());

	_P_UpdateFixedBody();
	_UpdateRenderEntry();
	_OnMeshChanged();
}

void ORenderable::SetCollisionEnabled(bool collisionEnabled)
{
	if (collisionEnabled)
	{
		if (!_physics.IsValid())
		{
			_physics = GetWorld().CreateFixedBody(this);
			_P_InitFixedBody();
			_P_UpdateFixedBody();
		}
	}
	else
	{
		_physics.Release();
	}
}

void ORenderable::Read(ByteReader &reader, ObjectIOContext& ctx)
{
	WorldObject::Read(reader, ctx);

	const String *string;

	if (string = ctx.strings.TryGet(reader.Read_uint16()))
		SetModel(*string, ctx.context);
	
	if (string = ctx.strings.TryGet(reader.Read_uint16()))
		SetMaterial(*string, ctx.context);

	((Vector4&)_colour).Read(reader);
}

void ORenderable::Write(ByteWriter &writer, ObjectIOContext& ctx) const
{
	WorldObject::Write(writer, ctx);

	ModelManager* modelManager = ctx.context.GetPtr<ModelManager>();
	if (_model)
	{
		uint16 id = ctx.strings.Add(modelManager->FindNameOf(_model.Ptr()));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);

	MaterialManager* materialManager = ctx.context.GetPtr<MaterialManager>();
	if (!_materialIsDefault && _material)
	{
		uint16 id = ctx.strings.Add(materialManager->FindNameOf(_material.Ptr()));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);

	((const Vector4&)_colour).Write(writer);
}
