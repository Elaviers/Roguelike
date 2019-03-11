#pragma once
#include "GameObject.h"
#include "Engine.h"
#include "MaterialManager.h"
#include "ModelManager.h"

class ObjRenderable : public GameObject
{
private:
	const Model *_model;
	const Material *_material;

	bool _materialIsDefault;

public:
	GAMEOBJECT_FUNCS(ObjRenderable)
	
	ObjRenderable() : GameObject(FLAG_SAVEABLE), _model(nullptr), _material(nullptr), _materialIsDefault(false) {}
	virtual ~ObjRenderable() {}

	inline void SetModel(const String &name) { if (Engine::Instance().pModelManager) SetModel(Engine::Instance().pModelManager->Get(name)); }
	inline void SetModel(const Model *model) 
	{ 
		_model = model; 
		if (_model && model->defaultMaterial.GetLength() != 0) 
		{ 
			SetMaterial(model->defaultMaterial); 
			_materialIsDefault = true; 
		} 
	}

	inline void SetMaterial(const String &name) { if (Engine::Instance().pMaterialManager) SetMaterial(*Engine::Instance().pMaterialManager->Get(name)); }
	inline void SetMaterial(const Material *material) { _material = material; _materialIsDefault = false; }

	inline bool MaterialIsDefault() const { return _materialIsDefault; }

	virtual void Render() const override;

	virtual void GetCvars(CvarMap&) override;

	//These are for properties really
	String GetModelName() const;
	String GetMaterialName() const;

	//File IO
	virtual void WriteToFile(BufferIterator<byte>&, NumberedSet<String> &strings) const override;
	virtual void ReadFromFile(BufferIterator<byte>&, const NumberedSet<String> &strings) override;

	//Collision
	virtual const Collider* GetCollider() const override;

	//Other
	virtual Bounds GetBounds() const override 
	{
		if (_model) return _model->bounds;
		return Bounds();
	}
};
