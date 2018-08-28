#pragma once
#include "GameObject.h"
#include "MaterialManager.h"
#include "ModelManager.h"

class Renderable : public GameObject
{
private:
	const Model *_model;
	const Material *_material;

	static MaterialManager* _materialManager;
	static ModelManager* _modelManager;

public:
	Renderable() {}
	virtual ~Renderable() {}

	inline static void SetManagers(MaterialManager* materialManager, ModelManager* modelManager)
	{
		_materialManager = materialManager;
		_modelManager = modelManager;
	}

	inline void SetModel(const String &name) { if (_modelManager) SetModel(_modelManager->GetModel(name)); }
	inline void SetModel(const Model *model) { _model = model; if (model->defaultMaterial.GetLength() != 0) SetMaterial(model->defaultMaterial); }

	inline void SetMaterial(const String &name) { if (_materialManager) SetMaterial(_materialManager->Find(name)); }
	inline void SetMaterial(const Material *material) { _material = material; }

	virtual void Render() const override;

	virtual void GetProperties(ObjectProperties&) override;

	//These are for properties really
	String GetModelName() const;
	String GetMaterialName() const;
};
