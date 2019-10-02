#pragma once
#include "GameObject.hpp"
#include "Colour.hpp"
#include "Engine.hpp"
#include "MaterialManager.hpp"
#include "ModelManager.hpp"

class ObjRenderable : public GameObject
{
private:
	const Model *_model;
	const Material *_material;

	Colour _colour;

	bool _materialIsDefault;

public:
	GAMEOBJECT_FUNCS(ObjRenderable, ObjectIDS::RENDERABLE)
	
	ObjRenderable() : 
		GameObject(FLAG_SAVEABLE), 
		_model(nullptr), 
		_material(nullptr), 
		_colour(1.f, 1.f, 1.f),
		_materialIsDefault(false) 
	{}
	virtual ~ObjRenderable() {}

	inline const Colour& GetColour() const { return _colour; }

	inline const Material* GetMaterial() const { return _material; }
	inline const Model* GetModel() const { return _model; }

	inline void SetColour(const Colour& colour) { _colour = colour; }
	
	inline void SetMaterial(const String &name) { if (Engine::Instance().pMaterialManager) SetMaterial(Engine::Instance().pMaterialManager->Get(name)); }
	inline void SetMaterial(const Material *material) { _material = material; _materialIsDefault = false; }

	inline void SetModel(const String& name) { if (Engine::Instance().pModelManager) SetModel(Engine::Instance().pModelManager->Get(name)); }
	inline void SetModel(const Model* model)
	{
		_model = model;
		if (_model && model->GetDefaultMaterialName().GetLength() != 0)
		{
			SetMaterial(model->GetDefaultMaterialName());
			_materialIsDefault = true;
		}
	}

	inline bool MaterialIsDefault() const { return _materialIsDefault; }

	virtual void Render(EnumRenderChannel) const override;

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
