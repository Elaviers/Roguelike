#pragma once
#include "Entity.hpp"
#include "Colour.hpp"
#include "Engine.hpp"
#include "MaterialManager.hpp"
#include "ModelManager.hpp"

class EntRenderable : public Entity
{
protected:
	SharedPointer<const Model> _model;
	SharedPointer<const Material> _material;

	Colour _colour;

	bool _materialIsDefault;

	virtual void _OnModelChanged() {}

public:
	Entity_FUNCS(EntRenderable, EntityID::RENDERABLE)
	
	EntRenderable() : 
		Entity(Flags::SAVEABLE), 
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
	
	void SetMaterial(const String &name) { if (Engine::Instance().pMaterialManager) SetMaterial(Engine::Instance().pMaterialManager->Get(name)); }
	void SetMaterial(const SharedPointer<const Material>& material) { _material = material; _materialIsDefault = false; }

	void SetModel(const String& name) { if (Engine::Instance().pModelManager) SetModel(Engine::Instance().pModelManager->Get(name)); }
	void SetModel(const SharedPointer<const Model>& model)
	{
		_model = model;
		if (_model && model->GetDefaultMaterialName().GetLength() != 0)
		{
			SetMaterial(model->GetDefaultMaterialName());
			_materialIsDefault = true;
		}

		_OnModelChanged();
	}

	bool MaterialIsDefault() const { return _materialIsDefault; }

	virtual void Render(RenderChannels) const override;

	virtual const PropertyCollection& GetProperties() override;

	//These are for properties really
	String GetModelName() const;
	String GetMaterialName() const;

	//File IO
	virtual void WriteData(BufferWriter<byte>&, NumberedSet<String> &strings) const override;
	virtual void ReadData(BufferReader<byte>&, const NumberedSet<String> &strings) override;

	//Collision
	virtual const Collider* GetCollider() const override;

	//Other
	virtual Bounds GetBounds() const override 
	{
		if (_model) return _model->GetBounds();
		return Bounds();
	}
};
