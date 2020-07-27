#pragma once
#include "Entity.hpp"
#include "ModelManager.hpp"
#include <ELCore/Context.hpp>
#include <ELGraphics/Colour.hpp>
#include <ELGraphics/MaterialManager.hpp>

class EntRenderable : public Entity
{
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
		_model(nullptr), 
		_material(nullptr), 
		_colour(1.f, 1.f, 1.f),
		_materialIsDefault(false) 
	{}
	virtual ~EntRenderable() {}

	const Colour& GetColour() const { return _colour; }

	const SharedPointer<const Material>& GetMaterial() const { return _material; }
	const SharedPointer<const Model>& GetModel() const { return _model; }

	void SetColour(const Colour& colour) { _colour = colour; }
	
	void SetMaterial(const String &name, const Context& ctx) { SetMaterial(ctx.GetPtr<MaterialManager>()->Get(name, ctx)); }
	void SetMaterial(const SharedPointer<const Material>& material) { _material = material; _materialIsDefault = false; }

	void SetModel(const String& name, const Context& ctx) { SetModel(ctx.GetPtr<ModelManager>()->Get(name, ctx), ctx); }
	void SetModel(const SharedPointer<const Model>& model, const Context& ctx)
	{
		_model = model;
		
		if (_model && _model->GetDefaultMaterialName().GetLength() != 0)
		{
			SetMaterial(model->GetDefaultMaterialName(), ctx);
			_materialIsDefault = true;
		}
		else SetMaterial(SharedPointer<const Material>());

		_OnMeshChanged();
	}

	bool MaterialIsDefault() const { return _materialIsDefault; }

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
