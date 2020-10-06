#pragma once
#include "Entity.hpp"
#include "ModelManager.hpp"
#include <ELCore/Context.hpp>
#include <ELGraphics/Colour.hpp>
#include <ELGraphics/MaterialManager.hpp>
#include <ELGraphics/RenderEntry.hpp>

class EntRenderable : public Entity
{
private:
	RenderEntry _renderEntry;

	bool _static; //If static, render entry only calculated when object moves. Static objects are less efficient when they move frequently.

	void _UpdateRenderEntry();

	float _age = 0.f; //probably temp?

protected:
	SharedPointer<const Model> _model;
	SharedPointer<const Material> _material;

	Colour _colour;

	bool _materialIsDefault;

	virtual void _OnMeshChanged() {}

public:
	Entity_FUNCS(EntRenderable, EEntityID::RENDERABLE)
	
	EntRenderable() : 
		Entity(EFlags::SAVEABLE),
		_renderEntry(ERenderChannels::SURFACE),
		_model(nullptr),
		_material(nullptr),
		_colour(1.f, 1.f, 1.f),
		_materialIsDefault(false),
		_static(false)
	{
		onTransformChanged += Callback(this, &EntRenderable::_UpdateRenderEntry);
	}

	EntRenderable(const EntRenderable& other) :
		Entity(other),
		_renderEntry(ERenderChannels::SURFACE),
		_model(other._model),
		_material(other._material),
		_colour(other._colour),
		_materialIsDefault(other._materialIsDefault),
		_static(other._static)
	{
		onTransformChanged += Callback(this, &EntRenderable::_UpdateRenderEntry);
		_UpdateRenderEntry();
	}

	virtual ~EntRenderable() {}

	const Colour& GetColour() const { return _colour; }

	bool MaterialIsDefault() const { return _materialIsDefault; }
	const SharedPointer<const Material>& GetMaterial() const { return _material; }
	const SharedPointer<const Model>& GetModel() const { return _model; }

	void SetStatic(bool s) { _static = s; if (_static) _UpdateRenderEntry(); }

	void SetColour(const Colour& colour) { _colour = colour; _UpdateRenderEntry(); }
	
	void SetMaterial(const String &name, const Context& ctx) { SetMaterial(ctx.GetPtr<MaterialManager>()->Get(name, ctx)); }
	void SetMaterial(const SharedPointer<const Material>& material) { _material = material; _materialIsDefault = false; _UpdateRenderEntry(); }

	void SetModel(const String& name, const Context& ctx) { SetModel(ctx.GetPtr<ModelManager>()->Get(name, ctx), ctx); }
	void SetModel(const SharedPointer<const Model>& model, const Context& ctx);

	virtual void Update(float deltaTime) override { _age += deltaTime; }
	virtual void Render(RenderQueue&) const override;

	virtual const PropertyCollection& GetProperties() override;

	//These are for properties really
	String GetModelName(const Context& ctx) const;
	String GetMaterialName(const Context& ctx) const;

	//File IO
	virtual void WriteData(ByteWriter&, NumberedSet<String> &strings, const Context& ctx) const override;
	virtual void ReadData(ByteReader&, const NumberedSet<String> &strings, const Context& ctx) override;

	//Collision
	virtual const Collider* GetCollider() const override;

	//Other
	virtual Bounds GetBounds() const override 
	{
		if (_model) return _model->GetMesh()->bounds;
		return Bounds();
	}
};
